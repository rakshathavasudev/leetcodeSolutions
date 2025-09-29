#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
using namespace std;

class TrafficLight {
    mutex mtx;
    int currentRoad; // 0 = road A, 1 = road B
public:
    TrafficLight() : currentRoad(0) {} // initially road A is green

    void carArrived(
        int carId,                 // id of this car
        int roadId,                // 0 for road A, 1 for road B
        int direction,             // just for printing
        function<void()> turnGreen,// switch the light
        function<void()> crossCar  // let the car cross
    ) {
        unique_lock<mutex> lock(mtx);

        // If light is not already green for this road, switch it
        if (roadId != currentRoad) {
            turnGreen();
            currentRoad = roadId;
        }

        // Car crosses safely
        crossCar();
    }
};

int main() {
    TrafficLight light;

    // Functions for printing
    auto turnGreen = [&](int roadId) {
        cout << "Traffic Light on Road " << (roadId == 0 ? "A" : "B") << " is Green\n";
    };

    auto crossCar = [&](int carId, int roadId, int direction) {
        cout << "Car " << carId << " Has Passed Road " 
             << (roadId == 0 ? "A" : "B") 
             << " In Direction " << direction << "\n";
    };

    // Sample test cars
    vector<int> cars       = {1, 3, 5, 2, 4};
    vector<int> roads      = {0, 0, 0, 1, 1}; // 0 = A, 1 = B
    vector<int> directions = {2, 1, 2, 4, 3};

    vector<thread> threads;

    for (int i = 0; i < cars.size(); i++) {
        threads.emplace_back([&, i]() {
            light.carArrived(
                cars[i], roads[i], directions[i],
                [&]() { turnGreen(roads[i]); },
                [&]() { crossCar(cars[i], roads[i], directions[i]); }
            );
        });
    }

    for (auto &t : threads) t.join();

    return 0;
}
