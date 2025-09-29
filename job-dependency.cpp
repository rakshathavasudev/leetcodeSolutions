#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
using namespace std;

struct Job {
    string jobId;
    function<void()> doWork;   // may throw on failure
};

class PipelineManager {
public:
    PipelineManager(const vector<Job>& jobs,
                    const vector<pair<string,string>>& deps,
                    size_t threads = thread::hardware_concurrency())
        : jobs_(jobs), threadCount_(max<size_t>(1, threads)) {

        for (auto &j : jobs_) {
            jobMap_[j.jobId] = &j;
            inDegree_[j.jobId] = 0;
        }
        // Build dependency graph
        for (auto &e : deps) {
            dependents_[e.first].push_back(e.second);
            inDegree_[e.second]++;
        }
    }

    bool execute() {
        {
            lock_guard<mutex> lk(mtx_);
            for (auto &kv : inDegree_)
                if (kv.second == 0) readyQ_.push(kv.first);
        }

        vector<thread> pool;
        for (size_t i = 0; i < threadCount_; ++i)
            pool.emplace_back(&PipelineManager::worker, this);

        for (auto &t : pool) t.join();
        return !failed_;
    }

private:
    const vector<Job>& jobs_;
    size_t threadCount_;
    unordered_map<string, const Job*> jobMap_;   // store const Job
    unordered_map<string, vector<string>> dependents_;
    unordered_map<string, int> inDegree_;

    queue<string> readyQ_;
    mutex mtx_;
    condition_variable cv_;
    atomic<bool> failed_{false};
    int finished_ = 0;

    void worker() {
        while (true) {
            string jobId;
            {
                unique_lock<mutex> lk(mtx_);
                cv_.wait(lk, [&]{
                    return failed_ || !readyQ_.empty()
                           || finished_ == (int)jobs_.size();
                });
                if (failed_ || finished_ == (int)jobs_.size()) return;
                if (readyQ_.empty()) continue;
                jobId = readyQ_.front();
                readyQ_.pop();
            }

            try {
                jobMap_[jobId]->doWork();
            } catch (...) {
                failed_ = true;
                cv_.notify_all();
                return;
            }

            {
                lock_guard<mutex> lk(mtx_);
                finished_++;
                for (auto &dep : dependents_[jobId])
                    if (--inDegree_[dep] == 0) readyQ_.push(dep);
            }
            cv_.notify_all();
        }
    }
};


int main() {
    vector<Job> jobs = {
        {"A", []{ cout << "A\n"; }},
        {"B", []{ cout << "B\n"; }},
        {"C", []{ cout << "C\n"; }},
        {"E", []{ cout << "E\n"; }},
        {"F", []{ cout << "F\n"; }},
        {"G", []{ cout << "G\n"; }},
        {"K", []{ cout << "K\n"; }},
        {"L", []{ cout << "L\n"; }},
        {"M", []{ cout << "M\n"; }}
    };

    vector<pair<string,string>> deps = {
        {"A","B"}, {"B","C"}, {"C","G"},
        {"A","E"}, {"E","F"}, {"F","G"},
        {"K","L"}, {"L","M"}
    };

    PipelineManager pm(jobs, deps, 4);
    bool ok = pm.execute();
    cout << (ok ? "Pipeline succeeded\n" : "Pipeline failed\n");
}
