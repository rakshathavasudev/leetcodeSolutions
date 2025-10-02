#include<bits/stdc++.h>
using namespace std;
//Puzzle solver


    
    
    int pieces=100;
    vector<bool> board(pieces,false);
    int placeCount=0;
    mutex mtx;
condition_variable cv;
    
void placepieces(int id){
        while(true){
             int curr;
            {
           unique_lock<mutex> lock(mtx);
            
            //cv.wait(lock,[this]{ return placeCount<=pieces; });
            if(placeCount>pieces) return;
            curr = placeCount;
            }
            this_thread::sleep_for(100ms);
            
            {
                unique_lock<mutex> lock(mtx);
                cv.wait(lock,[curr]{ return curr==0 || board[curr-1]; });
                board[curr]=true;
               
                placeCount++;
                cout<<id<<" Placed Piece" <<curr<< endl;
                 //curr=0;
                 cv.notify_all();
            }
        }
     
}
    
    


int main(){
     vector<thread> workers;
    for (int i = 0; i < 4; i++) { // 4 workers
        workers.emplace_back(placepieces, i+1);
    }

    for (auto& t : workers) {
        t.join();
    }

    cout << "Puzzle completed with " << placeCount << " pieces!" << endl;
    return 0;
    
}