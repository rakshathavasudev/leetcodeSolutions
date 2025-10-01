#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <semaphore.h>

std::queue<int> buffer;
const unsigned int MAX_SIZE = 5;
std::mutex mtx;
sem_t emptySlots;   // counts available slots
sem_t fullSlots;    // counts filled slots

void producer(int id) {
    for (int i = 0; i < 10; ++i) {
        sem_wait(&emptySlots);         // wait for free slot
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push(i);
            std::cout << "Producer " << id << " produced " << i << "\n";
        }
        sem_post(&fullSlots);          // signal item available
    }
}

void consumer(int id) {
    for (int i = 0; i < 10; ++i) {
        sem_wait(&fullSlots);          // wait for item
        int item;
        {
            std::lock_guard<std::mutex> lock(mtx);
            item = buffer.front();
            buffer.pop();
            std::cout << "Consumer " << id << " consumed " << item << "\n";
        }
        sem_post(&emptySlots);         // signal slot free
    }
}

int main() {
    sem_init(&emptySlots, 0, MAX_SIZE);
    sem_init(&fullSlots, 0, 0);

    std::thread p1(producer, 1), p2(producer, 2);
    std::thread c1(consumer, 1), c2(consumer, 2);

    p1.join(); p2.join();
    c1.join(); c2.join();

    sem_destroy(&emptySlots);
    sem_destroy(&fullSlots);
    return 0;
}
