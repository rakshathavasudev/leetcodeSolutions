#include <bits/stdc++.h>
using namespace std;

// ------------------------------------------------------
// Example f(N) : usage time lookup
// ------------------------------------------------------
int f(const string &name, const unordered_map<string,int> &times) {
    auto it = times.find(name);
    return (it != times.end()) ? it->second : 0;
}

// ------------------------------------------------------
// Core scheduling function
// ------------------------------------------------------
struct Interval {
    int start, end;
    vector<string> group;
};

vector<Interval>
bathroomSchedule(vector<string> queue,
                 const unordered_map<string,int> &times)
{
    vector<Interval> result;
    int currentTime = 0;

    while (!queue.empty()) {
        char party = queue.front()[0];        // 'D' or 'R'
        vector<string> group;

        // Always take the first person of this party
        group.push_back(queue.front());
        queue.erase(queue.begin());

        // Look ahead to fill up to 3 of same party
        size_t i = 0;
        while (group.size() < 3 && i < queue.size()) {
            if (queue[i][0] == party) {
                group.push_back(queue[i]);
                queue.erase(queue.begin() + i);
            } else {
                ++i;
            }
        }

        // Duration is determined by the slowest person
        int duration = 0;
        for (auto &p : group)
            duration = max(duration, f(p, times));

        result.push_back({currentTime, currentTime + duration, group});
        currentTime += duration;
    }

    return result;
}

// ------------------------------------------------------
// Demo / Test
// ------------------------------------------------------
int main() {
    vector<string> queue1 = {"D1","D2","R1","R2","D3","D4","R3"};
    unordered_map<string,int> times1 =
        {{"D1",3},{"D2",4},{"R1",5},{"R2",3},{"D3",2},{"D4",6},{"R3",4}};

    auto schedule = bathroomSchedule(queue1, times1);

    for (auto &slot : schedule) {
        cout << "Time " << slot.start << "-" << slot.end << "s: Bathroom used by [";
        for (size_t i=0;i<slot.group.size();++i){
            cout << slot.group[i];
            if (i+1<slot.group.size()) cout << ", ";
        }
        cout << "]\n";
    }
    return 0;
}

