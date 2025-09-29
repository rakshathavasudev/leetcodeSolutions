#include <bits/stdc++.h>
using namespace std;

class UnisexBathroom {
private:
    int male = 0;          // currently waiting males
    int female = 0;        // currently waiting females
    int count = 0;         // people inside
    const int cap = 3;
    char curr = 'N';       // 'M', 'F', or 'N' (empty)
    char nextTurn = 'M';   // whose turn when empty
    mutex m1;
    condition_variable cv;

public:
    void maleBathroom(const string& name) {
        unique_lock<mutex> lock(m1);
        ++male;  // one more male waiting
        cv.wait(lock, [this] {
            // males may enter if:
            // 1. males already inside and capacity not full
            // 2. bathroom empty AND it's males' turn
            return (curr == 'M' && count < cap) ||
                   (count == 0 && nextTurn == 'M');
        });
        --male;
        curr = 'M';
        ++count;
        cout << name << " Entering bathroom\n";
        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(200));

        lock.lock();
        --count;
        cout << name << " Leaving bathroom\n";
        if (count == 0) {
            curr = 'N';
            // give the other gender a turn if any are waiting
            if (female > 0) nextTurn = 'F';
            else if (male > 0) nextTurn = 'M';
        }
        lock.unlock();
        cv.notify_all();
    }

    void femaleBathroom(const string& name) {
        unique_lock<mutex> lock(m1);
        ++female; // one more female waiting
        cv.wait(lock, [this] {
            return (curr == 'F' && count < cap) ||
                   (count == 0 && nextTurn == 'F');
        });
        --female;
        curr = 'F';
        ++count;
        cout << name << " Entering bathroom\n";
        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(200));

        lock.lock();
        --count;
        cout << name << " Leaving bathroom\n";
        if (count == 0) {
            curr = 'N';
            if (male > 0) nextTurn = 'M';
            else if (female > 0) nextTurn = 'F';
        }
        lock.unlock();
        cv.notify_all();
    }
};

int main() {
    vector<thread> th;
    UnisexBathroom bath;
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Raksh1");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Raksh2");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Raksh3");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Raksh4");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Raksh5");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Raksh6");
    th.emplace_back(&UnisexBathroom::maleBathroom,   &bath, "Aditya");
    th.emplace_back(&UnisexBathroom::maleBathroom,   &bath, "Raj");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Priya");
    th.emplace_back(&UnisexBathroom::femaleBathroom, &bath, "Anita");

    for (auto &t : th) t.join();
    return 0;
}
