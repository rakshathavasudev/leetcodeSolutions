

class WebCrawler{
    
    set<string> visited;
    queue<string> q;
    mutex m;
    condition_variable cv;
    bool done = false;
    string hostname;
    int num_t;
    
public:

     void worker(){
         string cur;
         while(true){
             
             {
              lock_guard<mutex> lock(m);
              cv.wait(m,[]{ return !q.empty() || done; });
              if(done && q.empty()) return;
              
              cur = q.front();
              q.pop();
             }
             
             for(auto& url: getUrls(cur)){
                 lock_guard<mutex> l(m2);
                 if(getHostname(url)==hostname){
                     if(visited.find(url)==visited.end()){
                         visited.insert(url);
                         q.push(url);
                         cv.notify_one();
                     }
                 }
             }
         }
         
    }
    
    set<string> crawler(string seedurl){
        hostname = getHostname(seedurl);
        
        {
                lock_guard<mutex> lock(m);
                q.push(seedurl);
                visited.insert(seedurl);
            
        }
        
        vector<threads> th;
        for(int i=0;i<num_t;i++){
            th.emplace_back(&WebCrawler::worker,this);
        }
        
        
        
        while(true){
            {
                 lock_guard<mutex> lock(m);
                 if(q.empty()){
                     done=true;
                      notify_all();
                      break;
                 }
                
            }
            this_thread::sleep_for(20ms);
        }
         for (auto& t : threads) t.join();
        return visited;
        
}
    
    
};


