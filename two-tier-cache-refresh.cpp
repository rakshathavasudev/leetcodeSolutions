#include<bits/stdc++.h>
using namespace std;

struct Value{
    
    string data;
    chrono::steady_clock::time_point expiry;
    bool valid() const{
        return chrono::steady_clock::now() < expiry;
    }
};

class LocalCache{
    
    unordered_map<string,Value> cache;
    shared_mutex m;
    
public:

    bool hasValid(const string& key){
        shared_lock lk(m);
        auto it = cache.find(key);
        return it!=cache.end() && it.second->valid();
    }
    
    Value get(string& key){
         shared_lock lk(m);
         return cache[key];
    } 
    
    void set(string& key,const string& data, int ttlSec=30){
        shared_lock lk(m); 
        cache[key] = {data,chrono::steady_clock::now() + chrono::seconds(ttlSec)};
    }
    
};


class DisributedCache{
    
    unordered_map<string,mutex> locks;
    unordered_map<string,Value> cache;

public:

    Value get(string& key){
        return cache[key];
    }
    
    void set(string& key){
         cache[key] = {data,chrono::steady_clock::now() + chrono::seconds(ttlSec)};
    }
    
    bool trylock(const string& key){
        return locks[key].try_lock();
    }
    
    void unlock(const string& key){
        locks[key].unlock();
    }
    
};


class TwoTierCache{
    
    unordred_map<string, mutex> locallocks;
    LocalCache local;
    DistributedCache& dist;
    
    
  public:
    
        TwoTierCache(DistributedCache& d) : dist(d) {}
        
        string get(string& key){
            
            if(local.hasValid(key) return local.get(key));
            
            auto val = dist.get(key);
            if(val.valid()){
                local.set(key,val.data);
                return val.data;
            }
            
            //have to refersh locks/cache
            unique_lock<mutex> lc(locallocks[key]);
            if (local.hasValid(key)) return local.get(key).data;
            
            if(dist.trylock(key)){
                string fresh = loadfromdb(key);
                dist.set(key,fresh);
                dist.unlock(key);
                local.set(key,fresh);
                return fresh;
            }else{
                this_thread::sleep_for(100ms);
                auto retry = dist.get(key);
                local.set(key,retry);
                return retry;
            }
        
        
    }
    
    string loadfromdb(string key){
        return "VALUE+_" + key;
    }
    
    
};

int main() {
    DistributedCache dist;
    TwoTierCache cache(dist);

    auto worker = [&](string key, int id) {
        string val = cache.get(key);
        cout << "Thread " << id << " got: " << val << "\n";
    };

    vector<thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back(worker, "A", i);
    }
    for (auto& t : threads) t.join();

    return 0;
}