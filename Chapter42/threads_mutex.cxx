#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <chrono>

using namespace std;

map<string, string> g_pages;
mutex g_pages_mutex;


mutex cout_mutex;

void save_page(const string &url)
{
    // simulate a long page fetch
    this_thread::sleep_for(chrono::seconds(2));
    string result = "fake content";

    lock_guard<mutex> guard(g_pages_mutex);
    g_pages[url] = result;
}


void hello()
{
    cout_mutex.lock();
    cout << "Hello,";
    cout_mutex.unlock();
}

void world()
{
    cout_mutex.lock();
    cout << "World!";
    cout_mutex.unlock();
}

// recursive_mutex
class X {
    recursive_mutex m;
    string shared;
public:
    void fun1(){
        lock_guard<recursive_mutex> lk(m);
        shared = "fun1";
        cout << "in fun1, shared variable is now " << shared << '\n'; 
    }
    void fun2(){
        lock_guard<recursive_mutex> lk(m);
        shared= "fun2";
        cout << "in fun2, shared variable is now " << shared << '\n';
        fun1(); // recursive lock becomes useful here
        cout << "back in fun2, shared variable is " << shared << '\n';
    }

};
int main()
{
    thread t1{hello};
    thread t2{world};

    t1.join();
    t2.join();

    cout << endl;
    thread t3(save_page, "http://foo");
    thread t4(save_page, "http://bar");
    t3.join();
    t4.join();

    //safe to access g_pages without lock now, as the threads are joined
    for (const auto &pair : g_pages){
        cout << pair.first << " => " << pair.second << '\n';
    }

    X x;
    thread t5(&X::fun1, &x);
    thread t6(&X::fun2, &x);
    t5.join();
    t6.join();

}

