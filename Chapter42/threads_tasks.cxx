#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <chrono>

using namespace std;

void f(){
    cout << "f\n";
}

void g(){
    cout << "g\n";
}

struct guarded_thread : thread {
    using thread::thread;
    ~guarded_thread() { if (joinable()) join(); }
};

void run2(int i, int n)
{
    guarded_thread t1{f};
    guarded_thread t2;

    vector<bool> v;

    if (i < n){
        guarded_thread t3{g};

        t2 = move(t3);
    }
    v[i] = true;
}

void tick(int n)
{
    for (int i=0; i!=n; ++i){
        this_thread::sleep_for(chrono::seconds{1});
        cout << "Alive!" << i << endl;
    }
    cout << "Lift off!\n";
}


int main()
{
    guarded_thread timer{tick,10};
    timer.join();

    int i=0,n=2;
    run2(i,n);
}
