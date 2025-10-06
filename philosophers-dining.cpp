#include<bits/stdc++.h>
using namespace std;

class DiningPhilosophers{
    
    mutex forks[5];    
  
public:

    void philosophersEating(int id){
        
        int left = id;
        int right = (id+1) % 5;
        
        int first = min(left,right);
        int second = max(left,right);
     
         
          // Picking up forks
            cout << "Philosopher " << id << " is hungry \n";

            forks[first].lock();
            cout << "Philosopher " << id << " picked up fork  \n" << first << "\n";

            forks[second].lock();
        cout << "Philosopher " << id << " picked up fork \n" << second << "\n";
       
        cout << "Philosopher " << id << " is eating 🍝\n";
            this_thread::sleep_for(chrono::milliseconds(800 + rand() % 400));     
        
        
    //eat
    forks[first].unlock();
    forks[second].unlock();
     cout << "Philosopher " << id << " put down forks and is done eating ✅\n\n";

            // Pause before next cycle
            this_thread::sleep_for(chrono::milliseconds(400));
        
    
         
     }   
       
    
};

int main() {
    //srand(time(0));
    DiningPhilosophers dp;

    vector<thread> philosophers;
    for (int i = 0; i < 5; ++i) {
        philosophers.emplace_back(&DiningPhilosophers::philosophersEating, &dp, i);
    }

    // Let simulation run for a few seconds
    this_thread::sleep_for(chrono::seconds(10));

    cout << "\n[Main] Ending simulation...\n";

    // Detach threads (not ideal in real-world, but for test simulation it's fine)
    for (auto &t : philosophers) t.detach();

    return 0;
}
