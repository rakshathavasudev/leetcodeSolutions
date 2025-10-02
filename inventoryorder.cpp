#include <bits/stdc++.h>
#include <mutex>
#include <condition_variable>
using namespace std;

struct Order {
    vector<pair<string,int>> items;  // list of (item, qty)
};

class WareHouse{
     
    unordered_map<string,int> inventorylist;
    bool fullfill;
    queue<Order> backorders;
    mutex m;
    condition_variable cv;
 
 public:
 
    WareHouse(vector<pair<string,int>> stock) {
        for (auto &s : stock) inventorylist[s.first] = s.second;
    }
    
    void placeOrder(Order order){
        unique_lock<mutex> lock(m);
        
        for(auto& req: order.items){
            if(inventorylist[req.first] < req.second){
                fullfill = false;
                break;
            }
        }
        
        if(fullfill){
             for(auto& req: order.items){
            inventorylist[req.first]-=req.second;
        }
        cout<< "Fullfilled \n";
        }
        else{
            cout<< "Back order \n";
            backorders.push(order);
        }
        lock.unlock();
        cv.notify_all();
        
        
    }
    void printInventory() {
        unique_lock<mutex> lock(m);
        cout << "Inventory:\n";
        for (auto &kv : inventorylist) {
            cout << "  " << kv.first << " = " << kv.second << "\n";
        }
    }
  
};


int main() {
    WareHouse wh({{"apple", 10}, {"banana", 5}, {"orange", 0}});

    //wh.printInventory();

    Order o1 = {{{"apple", 3}, {"banana", 2}}};
    Order o2 = {{{"orange", 2}}}; // needs backorder
    vector<Order> orderList = {o1, o2};
    wh.printInventory();
    vector<thread> th;
    for(int i=0;i<10;i++){
        Order o = orderList[i% orderList.size()];
        th.emplace_back(&WareHouse::placeOrder,&wh,o);
        
        
    }
    wh.printInventory();
    
    for(auto& t: th) t.join();
    return 0;

   
}