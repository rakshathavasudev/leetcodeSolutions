You are tasked with simulating a barbershop with the following rules:

There is 1 barber and a waiting room with N chairs.

Customers arrive at random times.

If the barber is free, a customer immediately gets a haircut.

If the barber is busy and a chair is free, the customer waits in the waiting room.

If no chair is free, the customer leaves.

The barber sleeps when there are no customers and is woken up when a new customer arrives.

Goal:
Design a thread-safe solution so that multiple customer threads and one barber thread operate correctly, avoiding race conditions, deadlocks, and starvation.

N:10
shared states: Barber thread, queue of customers


Requirements
Functional

- Barber: thread, multiple customer threads
-queue of customers
-locks and mutexes to control allowing of customers in the room

Non-functional
-Fairness for customer? 
-No race condition: accessing chairs(customer) and for barber thread
-avoid any deadlock

#include <bits/stdc++.h>
using namespace std;

class BarberShop {
    condition_variable cvb, cvc;
    mutex m;
    queue<int> waiting;
    bool barberFree = true;
    const int n = 10; // number of chairs

public:
    void customerarrives(int id) {
        unique_lock<mutex> lock(m);

        if (waiting.size() == n) {
            cout << "Customer " << id << " leaves (no chairs free)\n";
            return;
        }

        waiting.push(id);
        cout << "Customer " << id << " waits.\n";

        // Wake up barber if sleeping
        cvb.notify_one();

        // Wait until it's this customer's turn and barber is free
        cvc.wait(lock, [&] {
            return !waiting.empty() && waiting.front() == id && barberFree;
        });

        // Get haircut
        barberFree = false;
        waiting.pop();
        lock.unlock();

        cout << "Customer " << id << " getting haircut\n";
        this_thread::sleep_for(chrono::milliseconds(300));
        cout << "Customer " << id << " done\n";

        // Notify barber and other waiting customers
        lock.lock();
        barberFree = true;
        cvc.notify_all();
    }

    void barberworks() {
        while(true){
            unique_lock<mutex> lock(m);
            cvb.wait(lock, [&] { return !waiting.empty(); });

            cout << "Barber ready for next customer.\n";
            cvc.notify_all(); // wake up customers
            lock.unlock();

            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
};

int main() {
    BarberShop bs;
    vector<thread> bth;
    
    for(int i=0;i<3;i++){
          bth.emplace_back(&BarberShop::barberworks, &bs);
    }

    //for (auto &t : bth) t.join();

    vector<thread> cth;
    for (int i = 0; i < 20; i++) {
        this_thread::sleep_for(chrono::milliseconds(100));
        cth.emplace_back(&BarberShop::customerarrives, &bs, i);
    }
   
    for (auto &t : cth) t.join();

   for (auto &t : bth) t.detach(); // barber keeps running (demo)
    return 0;
}
