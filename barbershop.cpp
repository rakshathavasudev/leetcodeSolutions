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


High-level-design

thread pool: customers, 1 barber thread

class called BarberShop
	
  
  -Initially barber thread active
  -check for chairs if free and waiting customers"
  
  		-customerarrives:
      
      		-check if free chairs
          -push it to the waiting queue
          -notify barber
          
        
      barberworks
      	
      		-customers from waiting queue 
          -keep count occupied chairs
          		-notify busy
          - remove the customer the queue
          -increase chair count
          
          -if the queue is empty then current thread would be sleeping
          			
          	
          
class BarberShop {
private:
    int chairs;
    queue<int> waitingRoom;
    mutex m;
    condition_variable barberCV;
    condition_variable customerCV;
    bool barberBusy = false;

public:
    BarberShop(int n) : chairs(n) {}

    void customerArrives(int id) {
        unique_lock<mutex> lock(m);

        if (waitingRoom.size() == chairs) {
            cout << "Customer " << id << " leaves (no chair).\n";
            return;
        }

        waitingRoom.push(id);
        cout << "Customer " << id << " waits.\n";

        // Wake barber if sleeping
        barberCV.notify_one();

        // Wait until barber ready for this customer
        customerCV.wait(lock, [&] {
            return waitingRoom.front() == id && !barberBusy;
        });

        // This customer is now being served
        barberBusy = true;
        waitingRoom.pop();
    }

    void barberWork() {
        while (true) {
            unique_lock<mutex> lock(m);

            // Wait until at least one customer available
            barberCV.wait(lock, [&] { return !waitingRoom.empty(); });

            int cid = waitingRoom.front();
            barberBusy = false;

            // Signal the chosen customer
            customerCV.notify_all();

            lock.unlock();

            // Cut hair
            cout << "Barber cutting hair for " << cid << "\n";
            this_thread::sleep_for(chrono::milliseconds(500));
            cout << "Barber finished with " << cid << "\n";

            lock.lock();
            barberBusy = false;
            lock.unlock();
        }
    }
};


      



































