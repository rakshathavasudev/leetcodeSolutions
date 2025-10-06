#include<bits/stdc++.h>
using namespace std;

class RateLimiter{
    
    double rate;
    double tokens;
    double cap;
    mutex m;
    chrono::steady_clock::time_point last;

public:    
    RateLimiter(double rateLimit, double capacity) :
        rate(rateLimit) , cap(capacity) , tokens(capacity) ,
        last(chrono::steady_clock::now())
    {
        
    }
    

    
    bool canAccept(){
        unique_lock<mutex> lock(m);
        
        auto now = chrono::steady_clock::now();
        double elapsed = chrono::duration<double>(now-last).count();
        last = now;
        tokens = min(cap, tokens+elapsed * rate);
        
        if(tokens >= 1.0){
            tokens-=1;
            return true;
        }
        return false;
        
    }
    
};

int main(){
    
    RateLimiter rl(5.0,10.0);
    
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




