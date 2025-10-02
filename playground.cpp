#include <bits/stdc++.h>
using namespace std;

class FootballPlayground{
    
    int insideCount=0;
    string currentteam="";
    queue<string> teamqueue;
    unordered_map<string,int> waiting;
    mutex m;
    condition_variable cv;
    
    enterground(string team){
        
        unique_lock<mutex> lock(m);
        
        waiting[team]++;
        if(waiting[team]==1)  teamqueue.push(team);
        cv.wait(lock,[this]{ 
        bool same = (!currentteam.empty() && currentteam == team);
        bool emptyground = (insideCount==0 && teamqueue.front()==team)
        
    return (same || emptyground) && insideCount < 10; });
    
    waiting[team]--;
    if(insideCount==0) currentteam = team;
    
    insideCount++;

    }
    
    leaveground()
{
         unique_lock<mutex> lock(m);
         insideCount--;
         
         if(insideCount==0){
             currentteam.clear();
             if(!teamqueue.empty() && teamqueue.front()==team && waiting[team]==0) {
                 teamqueue.pop();
             }else cv.notify_all();
         }else{
             cv.notify_all();         }
    
        }   
    
};


int main(){
    
    vector<thread> teams;
    return 0;
    
}













