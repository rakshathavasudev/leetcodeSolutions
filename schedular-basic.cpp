You are to design a Scheduler class that:

Accepts tasks submitted by multiple clients.

Runs them, but should not block the client submitting the task.

The scheduler method (e.g. schedule(task)) must return immediately — i.e., it should not wait for task completion.

There is no upper limit on threads (infinite or unbounded thread pool assumption).

So essentially, you want something like a non-blocking, fire-and-forget scheduler — where clients submit jobs instantly, and the scheduler ensures they eventually run.


#include <iostream>
#include <thread>
#include <functional>
#include <atomic>

class Scheduler {
    std::atomic<bool> running;

public:
    Scheduler() : running(true) {}

    void stop() { running.store(false); }

    bool schedule(std::function<void()> task) {
        if (!running.load()) return false;

        // Spawn a detached thread immediately
        std::thread([task]() {
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "Task error: " << e.what() << std::endl;
            }
        }).detach();

        // Return immediately — no blocking
        return true;
    }
};



int main() {
    Scheduler s;

    for (int i = 0; i < 5; ++i) {
        s.schedule([i]() {
            std::cout << "Task " << i << " running on thread "
                      << std::this_thread::get_id() << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    s.stop();
}
