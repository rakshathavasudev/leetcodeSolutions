#include<bits/stdc++.h>
using namespace std;

struct Node{
    int data;
    Node* next;
    Node(int d): data(d) , next(nullptr) {}
    
};

class LockStack{
    
    mutex m;
    Node* head;

public:
        LockStack(): head(nullptr) {} 

        void push(int val){
           { 
                lock_guard<mutex> lock(m);
                Node* newnode = new Node(val);
                newnode->next = head;
                head = newnode;
            
           }
        }
        
        bool pop(){
            {
               lock_guard<mutex> lock(m); 
               if(head==nullptr) return false;
               
               Node* n = head;
               head = head->next;
               delete n;
               return true;
  
            }
    
        }
        
        int peek(){
            {
            lock_guard<mutex> lock(m); 
               if(head==nullptr) return -1;
              return head->data;
            }
        }
        
          bool empty() {
        lock_guard<mutex> lock(m);
        return head == nullptr;
    }
    
    
};

int main(){
     LockStack mtStack;mutex mtx;
     bool t1_done = false;
    condition_variable cv;
    
   auto pushTask1 = [&] {
    for (int i = 0; i < 5; i++) mtStack.push(i);
    {
        std::lock_guard<std::mutex> lock(mtx);
        t1_done = true;
    }
    cv.notify_one();
};

auto pushTask2 = [&] {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]{ return t1_done; }); // wait for t1
    for (int i = 100; i < 105; i++) mtStack.push(i);
};
    thread t1(pushTask1);
    thread t2(pushTask2);

    t1.join();
    t2.join();

    cout << "Multi-thread stack peek: " << mtStack.peek() << endl;

    cout << "Popping multi-threaded stack:\n";
    while (!mtStack.empty()) {
        cout << mtStack.peek() << " ";
        mtStack.pop();
    }
    cout << endl;

    return 0;
}









