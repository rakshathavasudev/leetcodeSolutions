#include <bits/stdc++.h>
#include <shared_mutex>
using namespace std;

class SnapshotArray {
public:
    atomic<int> snap_id;
    vector<map<int,int>> history; 
    deque<mutex> locks; // one mutex per index

    SnapshotArray(int length) {
        snap_id = 0;
        history.resize(length);
        locks.resize(length);
        for (int i = 0; i < length; i++) {
            history[i][0] = 0; // initial value
        }
    }

    void set(int index, int val) {
        lock_guard<mutex> lg(locks[index]);
        history[index][snap_id.load()] = val;
    }

    int snap() {
        return snap_id.fetch_add(1); // atomic increment
    }

    int get(int index, int id) {
        lock_guard<mutex> lg(locks[index]);
        auto &m = history[index];
        auto it = m.upper_bound(id);
        if (it == m.begin()) return 0;
        --it;
        return it->second;
    }
};


int main() {
    SnapshotArray sa(5);

    thread t1([&](){
        sa.set(0, 10);
        cout << "snap id from t1: " << sa.snap() << endl;
    });

    thread t2([&](){
        sa.set(0, 20);
        cout << "snap id from t2: " << sa.snap() << endl;
    });

    t1.join();
    t2.join();

    cout << "get(0,0) = " << sa.get(0,0) << endl;
    cout << "get(1,0) = " << sa.get(1,0) << endl;
}
