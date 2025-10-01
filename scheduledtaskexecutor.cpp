#include <queue>
#include <thread>
#include <chrono>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <iostream>

enum class TaskType { RUN_ONCE, FIXED_RATE, FIXED_DELAY };

struct ScheduledTask {
    std::function<void()> func;
    std::chrono::steady_clock::time_point exec_time;
    TaskType type;
    std::chrono::milliseconds period_or_delay;
    bool operator<(const ScheduledTask& other) const {
        // priority_queue is max-heap, invert comparison for min-heap
        return exec_time > other.exec_time;
    }
};

class ScheduledExecutorService {
    std::priority_queue<ScheduledTask> task_queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
    std::thread worker;
public:
    ScheduledExecutorService() {
        worker = std::thread([this]() { this->run(); });
    }

    ~ScheduledExecutorService() {
        {
            std::lock_guard<std::mutex> lk(mtx);
            stop = true;
        }
        cv.notify_all();
        worker.join();
    }

    void schedule(std::function<void()> func, int delay_ms) {
        add_task({func, std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms), TaskType::RUN_ONCE, std::chrono::milliseconds(delay_ms)});
    }

    void scheduleAtFixedRate(std::function<void()> func, int initial_ms, int period_ms) {
        add_task({func, std::chrono::steady_clock::now() + std::chrono::milliseconds(initial_ms), TaskType::FIXED_RATE, std::chrono::milliseconds(period_ms)});
    }

    void scheduleWithFixedDelay(std::function<void()> func, int initial_ms, int delay_ms) {
        add_task({func, std::chrono::steady_clock::now() + std::chrono::milliseconds(initial_ms), TaskType::FIXED_DELAY, std::chrono::milliseconds(delay_ms)});
    }

private:
    void add_task(const ScheduledTask& task) {
        std::lock_guard<std::mutex> lk(mtx);
        task_queue.push(task);
        cv.notify_all();
    }

    void run() {
        std::unique_lock<std::mutex> lk(mtx);
        while (!stop) {
            if (task_queue.empty()) {
                cv.wait(lk);
                continue;
            }
            auto now = std::chrono::steady_clock::now();
            auto next_task = task_queue.top();
            if (now >= next_task.exec_time) {
                task_queue.pop();
                lk.unlock();
                next_task.func();
                lk.lock();
                if (next_task.type == TaskType::FIXED_RATE) {
                    add_task({next_task.func, next_task.exec_time + next_task.period_or_delay, next_task.type, next_task.period_or_delay});
                } else if (next_task.type == TaskType::FIXED_DELAY) {
                    add_task({next_task.func, std::chrono::steady_clock::now() + next_task.period_or_delay, next_task.type, next_task.period_or_delay});
                }
            } else {
                cv.wait_until(lk, next_task.exec_time);
            }
        }
    }
};

int main() {
    ScheduledExecutorService scheduler;
    scheduler.schedule([]() { std::cout << "One shot!\n"; }, 1000); // 1s delay
    scheduler.scheduleAtFixedRate([]() { std::cout << "Fixed rate!\n"; }, 500, 2000); // start after 0.5s, repeat every 2s
    scheduler.scheduleWithFixedDelay([]() { std::cout << "Fixed delay!\n"; }, 500, 3000); // start after 0.5s, repeat 3s after finish
    std::this_thread::sleep_for(std::chrono::seconds(10)); // let tasks run
    // Scheduler destructor will clean up
    return 0;
}
