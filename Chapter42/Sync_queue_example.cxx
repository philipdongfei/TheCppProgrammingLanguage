#include <list>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <system_error>

template<typename T>
class Sync_queue{
public:
    void put(const T& val)
    {
        std::lock_guard<std::mutex> lck(mtx);
        q.push_back(val);
        cond.notify_one();

    }

    void put(T val, std::chrono::steady_clock::duration d, int count=10)
    {
        std::unique_lock<std::mutex> lck(mtx);
        n = count;
        //auto sizelambda = [this](auto n){ return q.size() < n; };
        //bool not_full = cond.wait_for(lck, d, sizelambda(count));

        bool not_full = cond.wait_for(lck, d ,[this](){return q.size() < n;});
        if (not_full){
            q.push_back(val);
            cond.notify_one();
        }
        else {
            cond.notify_all();
            //try
            {
                throw std::system_error(EDOM, std::generic_category(), "Sync_queue: put() timeout");

            }
#if 0
            catch (const std::system_error& ex)
            {
                std::cout << ex.code() << '\n';
                std::cout << ex.code().message() << '\n';
                std::cout << ex.what() << '\n';
            }
#endif
            
        }
    }
    //void put(T&& val);
    void get(T& val)
    {
        std::unique_lock<std::mutex> lck(mtx);
        cond.wait(lck, [this]{return !q.empty();});
        val=q.front();
        q.pop_front();

    }
    
    void get(T& val, std::chrono::steady_clock::duration d)
    {
        
        std::unique_lock<std::mutex> lck(mtx);
        bool not_empty = cond.wait_for(lck,d,[this]{return !q.empty();});
        if (not_empty){
            val = q.front();
            q.pop_front();
        }
        else {

            try{
                throw std::system_error(EDOM, std::generic_category(), "Sync_queue: get() timeout");

            }
            catch (const std::system_error& ex)
            {
                std::cout << ex.code() << '\n';
                std::cout << ex.code().message() << '\n';
                std::cout << ex.what() << '\n';
            }
        }
    }
private:
    std::mutex mtx;
    std::condition_variable cond;
    std::list<T> q;
    int n;
};

#if 0
template<typename T>
void Sync_queue::put(const T& val)
{
    std::lock_guard<std::mutex> lck(mtx);
    q.push_back(val);
    cond.notify_one();
}

template<typename T>
void Sync_queue::get(T& val)
{
    std::unique_lock<std::mutex> lck(mtx);
    cond.wait(lck, [this]{return !q.empty();});
    val=q.front();
    q.pop_front();
}
#endif

Sync_queue<int> mq;
int g_m=0;
void producer()
{
    while(true){
        int m;
        m = g_m++;
        try{
            mq.put(m, std::chrono::milliseconds{200}, 10);
            std::this_thread::sleep_for(std::chrono::seconds(1)); 

        }
        catch (const std::system_error& ex)
        {
            std::cout << ex.code() << '\n';
            std::cout << ex.code().message() << '\n';
            std::cout << ex.what() << '\n';
        }
    }
}

void consumer()
{
    while(true){
        int m;
        mq.get(m, std::chrono::milliseconds{200});
        if (10 == m)
        {
            printf("consumer exit\n");
            return;

        }
        std::cout << "thread id: " << m << '\n';
    }
}

void signalHandler(int signum){
    if (SIGKILL==signum ||
            SIGINT == signum)
    {
        printf("signel get:%d\n", signum);
        exit(signum);

    }
}
int main()
{
    signal(SIGINT, signalHandler);
    signal(SIGKILL, signalHandler);
    std::thread t1(consumer);
    std::thread t2(producer);

    t1.join();
    t2.join();
    std::cout << "main end\n";
    return 0;
}
