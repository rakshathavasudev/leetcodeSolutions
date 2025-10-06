#include<bits/stdc++.h>
using namespace std;

class RateLimiter{
    
   int cap;
   queue<long long> q;
   long long expire;
    mutex m;
    
   long long now() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}


    chrono::steady_clock::time_point last;

public:    
    RateLimiter(long long time, int capacity) :
        expire(time) , cap(capacity)
        
    {
        
    }
    

    
    bool canAccept(){
        unique_lock<mutex> lock(m);
        
        long long curr = now();
        
        while(!q.empty() && curr-q.front() > expire){
            q.pop();
        }
        
        if(q.size() < cap){
            q.push(curr);
            return true;
        }
        return false;
    }
    
};

int main(){
    
    RateLimiter rl(1000,10);
    
    auto worker = [&](int id){
        for(int i=0;i<10;i++){
            if(rl.canAccept())
                    cout <<"Request "<<id << " allowed" <<endl;
            else
                cout <<"Request "<<id << " not allowed" <<endl;
        }
    };
    
    thread t1(worker,1);
    thread t2(worker,2);
    
    t1.join();
    t2.join();
    
    return 0;
}




