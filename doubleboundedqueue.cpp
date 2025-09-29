#include <bits/stdc++.h>
#include <semaphore.h>
using namespace std;

/**
 * A thread-safe bounded blocking queue implemented using POSIX semaphores.
 * - enqueue() blocks if the queue is full
 * - dequeue() blocks if the queue is empty
 */
 class BoundedBlockingQueue {
// public:
//     explicit BoundedBlockingQueue(int capacity) {
//         // Semaphore for available slots (initially = capacity)
//         sem_init(&availableSlots, 0, capacity);

//         // Semaphore for available items (initially = 0)
//         sem_init(&availableItems, 0, 0);
//     }

//     void enqueue(int element) {
//         // Wait for an available slot
//         sem_wait(&availableSlots);

//         {
//             lock_guard<mutex> lock(mtx);
//             q.push(element);
//         }

//         // Signal that one item is now available
//         sem_post(&availableItems);
//     }

//     int dequeue() {
//         // Wait for an available item
//         sem_wait(&availableItems);

//         int value;
//         {
//             lock_guard<mutex> lock(mtx);
//             value = q.front();
//             q.pop();
//         }

//         // Signal that one slot is now free
//         sem_post(&availableSlots);

//         return value;
//     }

//     int size() {
//         lock_guard<mutex> lock(mtx);
//         return static_cast<int>(q.size());
//     }

// private:
//     queue<int> q;
//     sem_t availableSlots;  // counts free slots
//     sem_t availableItems;  // counts items present
//     mutex mtx;             // protects queue access
// };
public:
    mutex m;
    condition_variable cv;
    deque<int> dq;
    size_t capacity;
    explicit BoundedBlockingQueue(size_t cap) : capacity(cap) {}

    void enqueue(int n) {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [this]{ return dq.size() < capacity; }); // wait for space
        dq.push_back(n);
        cv.notify_all();  // wake up any waiting consumer
    }

    int dequeue() {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [this]{ return !dq.empty(); }); // wait for item
        int val = dq.front();
        dq.pop_front();
        cv.notify_all();  // wake up any waiting producer
        return val;
    }

    size_t size() {
        lock_guard<mutex> lock(m);
        return dq.size();
        }
};
int main() {
    vector<string> operations = {
        "BoundedBlockingQueue","enqueue","dequeue","dequeue","enqueue",
        "enqueue","enqueue","enqueue","dequeue","size"
    };

    vector<vector<int>> arguments = {
        {2},{1},{},{},{0},{2},{3},{4},{},{}
    };

    vector<int> output;
    BoundedBlockingQueue* bbq = nullptr;
    vector<thread> threads;   // store all threads

    for (size_t i = 0; i < operations.size(); i++) {
        const string& op = operations[i];

        if (op == "BoundedBlockingQueue") {
            bbq = new BoundedBlockingQueue(arguments[i][0]);
        } 
        else if (op == "enqueue") {
            int val = arguments[i][0];
            threads.emplace_back([&, val]{ bbq->enqueue(val); });
        } 
        else if (op == "dequeue") {
            threads.emplace_back([&]{
                int val = bbq->dequeue();
                // protect output from race conditions
                static mutex out_mtx;
                lock_guard<mutex> lock(out_mtx);
                output.push_back(val);
            });
        }
        else if (op == "size") {
            threads.emplace_back([&]{
                int val = bbq->size();
                static mutex out_mtx;
                lock_guard<mutex> lock(out_mtx);
                output.push_back(val);
            });
        }
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    // Print results
    cout << "[";
    for (size_t i = 0; i < output.size(); i++) {
        cout << output[i];
        if (i + 1 < output.size()) cout << ",";
    }
    cout << "]\n";

    return 0;
}