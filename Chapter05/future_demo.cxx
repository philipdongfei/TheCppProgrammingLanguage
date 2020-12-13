#include <iostream>
#include <thread>
#include <future>

using namespace std;

void f(promise<int>& px) // a task: place the result in px 
{
    try{
        X res;
        //..compute a value fo res
        res = 6;
        cout << "f:res=" << res << endl;
        res++;
        px.set_value(res);
    }
    catch(...){
        px.set_exception(current_exception());
    }
}

void g(future<int>& fx) 
// a task: get the result from fx
{
    try {
        int v = fx.get();
        cout << v << endl;
    }
    catch(...){
        // handle error
        cout << "Exception from the thread: " << endl;
    }

}

int main(void)
{
    promise<int> px;  
    future<int> fx = px.get_future();
    thread t1{f, px};
    thread t2{g,fx};

    t1.join();
    t2.join();

    return 0;
}


