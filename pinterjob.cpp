
#include<bits/stdc++.h>
using namespace std;

class Printer{
    
    mutex m;
    condition_variable cvjob,cvdone;
    int active=0;
    bool stop=false;
    queue<int> q;
 public:
  
    void printer(){
        int job;
        while(true){
            {
            unique_lock<mutex> lock(m);
            
            cvjob.wait(lock,[this]{ return stop || !q.empty(); });
            
            if(stop && q.empty()) return;
            
            job = q.front();
            q.pop();
            
            active ++;
        }
          cout << "Printing job: "<< job << endl;
            {
                unique_lock<mutex> lock(m);
                active--;
                if(q.empty() && active ==0) cvdone.notify_all();
            }
            
        }

        
    }
    
    void schedule(int i){
        {
        unique_lock<mutex> lock(m);
        q.push(i);
        }
        cvjob.notify_one();
    }
    
    void wait(){
        unique_lock<mutex> lock(m);
        cvdone.wait(lock,[this]{ return active==0 && q.empty(); });
    }
    
     void shutdown() {
        {
            lock_guard<mutex> lock(m);
            stop = true;
        }
        cvjob.notify_all();
    }
};

int main() {
    Printer p;
    thread t(&Printer::printer, &p);


    for (int i = 1; i <= 5; i++) {
       p.schedule(i);
    }

    cout << "Main thread waiting until all jobs are done...\n";
    p.wait();
    cout << "All jobs finished!\n";

    p.shutdown();
    
    t.join();
}