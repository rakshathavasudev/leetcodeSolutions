#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
using namespace std;

// Represents a Task
struct Task {
    int id;
    int duration; // seconds to simulate CPU work
    Task(int i=0, int d=1) : id(i), duration(d) {}
};

// Worker Node that runs tasks from its queue in FIFO order
class WorkerNode {
public:
    WorkerNode(int nodeId) : id(nodeId), stopFlag(false) {
        worker = thread([this]{ workerLoop(); });
    }

    ~WorkerNode() {
        {
            unique_lock<mutex> lk(mtx);
            stopFlag = true;
            cv.notify_all();
        }
        if (worker.joinable()) worker.join();
    }

    void enqueue(Task t) {
        {
            lock_guard<mutex> lk(mtx);
            q.push(move(t));
        }
        cv.notify_one();
    }

    bool tryDequeue(Task &out) {
        lock_guard<mutex> lk(mtx);
        if (q.empty()) return false;
        out = move(q.front());
        q.pop();
        return true;
    }

    size_t queueSize() {
        lock_guard<mutex> lk(mtx);
        return q.size();
    }

    // For rebalancing (must hold lock externally)
    queue<Task>& getQueueUnsafe() { return q; }
    mutex& getMutex() { return mtx; }
    condition_variable& getCV() { return cv; }

    int getId() const { return id; }

private:
    int id;
    queue<Task> q;
    mutex mtx;
    condition_variable cv;
    thread worker;
    atomic<bool> stopFlag;

    void workerLoop() {
        while (true) {
            Task t;
            {
                unique_lock<mutex> lk(mtx);
                cv.wait(lk, [this]{ return stopFlag || !q.empty(); });
                if (stopFlag && q.empty()) return;
                t = move(q.front());
                q.pop();
            }
            // Simulate execution
            cout << "Node " << id << " running Task " << t.id 
                 << " for " << t.duration << "s\n";
            this_thread::sleep_for(chrono::seconds(t.duration));
            cout << "Node " << id << " finished Task " << t.id << "\n";
        }
    }
};

// Distributed Scheduler with N nodes
class Scheduler {
    int N;
    vector<unique_ptr<WorkerNode>> nodes;

public:
    Scheduler(int n) : N(n) {
        for (int i = 0; i < N; i++) {
            nodes.emplace_back(make_unique<WorkerNode>(i));
        }
    }

    // API 1: Run task manually (not usually needed since workers auto-run)
    void runTask(int nodeId, int taskId) {
        if (nodeId < 0 || nodeId >= N) return;
        nodes[nodeId]->enqueue(Task(taskId, 1));
    }

    // API 2: Add task to a node
    void addTask(int nodeId, int taskId, int duration=1) {
        if (nodeId < 0 || nodeId >= N) return;
        nodes[nodeId]->enqueue(Task(taskId, duration));
        cout << "Added Task " << taskId << " to Node " << nodeId << "\n";
    }

    // API 3: Rebalance tasks across nodes
    void rebalanceTask() {
        // Lock all nodes
        vector<unique_lock<mutex>> locks;
        locks.reserve(N);
        for (int i = 0; i < N; i++) locks.emplace_back(nodes[i]->getMutex());

        // Gather tasks
        vector<Task> allTasks;
        for (int i = 0; i < N; i++) {
            auto &q = nodes[i]->getQueueUnsafe();
            while (!q.empty()) {
                allTasks.push_back(move(q.front()));
                q.pop();
            }
        }

        if (allTasks.empty()) return;

        int total = allTasks.size();
        int avg = (total + N - 1) / N; // ceiling

        // Redistribute evenly
        int idx = 0;
        for (int i = 0; i < N; i++) {
            for (int c = 0; c < avg && idx < total; c++) {
                nodes[i]->getQueueUnsafe().push(allTasks[idx++]);
                nodes[i]->getCV().notify_one();
            }
        }

        cout << "Rebalanced " << total << " tasks\n";
    }

    // API 4: Print task queues
    void printTask() {
        for (int i = 0; i < N; i++) {
            cout << "Node " << i << " queue: ";
            queue<Task> temp;
            {
                lock_guard<mutex> lk(nodes[i]->getMutex());
                temp = nodes[i]->getQueueUnsafe();
            }
            while (!temp.empty()) {
                cout << temp.front().id << " ";
                temp.pop();
            }
            cout << "\n";
        }
    }
};

// Demo
int main() {
    Scheduler sched(3);

    sched.addTask(0, 101, 2);
    sched.addTask(0, 102, 3);
    sched.addTask(1, 201, 2);
    sched.addTask(2, 301, 1);
    sched.addTask(0, 103, 2);

    cout << "\nBefore rebalance:\n";
    sched.printTask();

    sched.rebalanceTask();

    cout << "\nAfter rebalance:\n";
    sched.printTask();

    // Let tasks run
    this_thread::sleep_for(chrono::seconds(10));
    return 0;
}
