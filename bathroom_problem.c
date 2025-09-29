
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class UnisexBathroom{
    
    condition_variable cv;
    int counter;
    mutex mtx;

public:

    UnisexBathroom(){
        counter=0;
    }
    
     void maleBathroom(string name){
        unique_lock<mutex> l(mtx);
        
        cv.wait(l,[&] { return (counter>=0 && counter<=2); });
        counter++;
        cout<<"Male " << name << " entered" << "\n";
        l.unlock();
        
        cout<<"Male " << name << " used" << "\n";
        l.lock();
        counter--;
        if(counter==0) 
        l.unlock();
        
        cv.notify_all();
        
    }
    
     void femaleBathroom(string name){
         unique_lock<mutex> l(mtx);
        
        cv.wait(l,[&] { return (counter<=0 && counter>=-2); });
        counter--;
         cout<<"Female " << name << " entered" << "\n";
        l.unlock();
        
         cout<<"Female " << name << " used" << "\n";
        l.lock();
        counter++;
        l.unlock();
        
        cv.notify_all();
        
    }
    
};


int main(){
    UnisexBathroom bath;
    thread t1(&UnisexBathroom::femaleBathroom, &bath, "Raksh");
    thread t2(&UnisexBathroom::femaleBathroom, &bath, "Raksh");
    thread t3(&UnisexBathroom::femaleBathroom, &bath, "Raksh");
    thread t4(&UnisexBathroom::femaleBathroom, &bath, "Raksh");
    thread t5(&UnisexBathroom::maleBathroom, &bath, "Aditya");
    thread t6(&UnisexBathroom::femaleBathroom, &bath, "Raksh");
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    
    
}