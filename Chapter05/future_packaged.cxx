#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <functional>
#include <numeric>


double accum(double* beg, double* end, double init)
{
    return std::accumulate(beg, end, init);
}

double comp2(std::vector<double>& v)
{
    using Task_type = double(double*,double*,double);

    std::packaged_task<Task_type> pt0{accum};
    std::packaged_task<Task_type> pt1{accum};

    std::future<double> f0{pt0.get_future()};
    std::future<double> f1{pt1.get_future()};

    double* first = &v[0];

    std::thread t1{move(pt0),first, first + v.size()/2,0};
    std::thread t2{move(pt1), first + v.size()/2,first + v.size(),0};

    t1.join();
    t2.join();

    return f0.get()+f1.get();
}

double comp4(std::vector<double>& v)
{
    
    if (v.size() < 10000) {
        double* first = &v[0];

        return accum(first,first + v.size(),0.0);
    }

    auto v0 = &v[0];
    auto sz = v.size();

    auto f0 = std::async(accum,v0,v0+sz/4,0.0);
    auto f1 = std::async(accum,v0+sz/4,v0+sz/2,0.0);
    auto f2 = std::async(accum,v0+sz/2,v0+sz*3/4,0.0);
    auto f3 = std::async(accum,v0+sz*3/4,v0+sz,0.0);

    return f0.get()+f1.get()+f2.get()+f3.get();

}

int main()
{
    // future from a packaged_task
    std::packaged_task<int()> task([]{return 7;}); //wrap the function
    std::future<int> f1 = task.get_future();
    std::thread t(std::move(task)); // launch on a thread

    //future from an async()
    std::future<int> f2 = std::async(std::launch::async, []{return 8;});

    // future from a promise
    std::promise<int> p;
    std::future<int> f3 = p.get_future();
    std::thread([&p]{p.set_value_at_thread_exit(9);}).detach();

    std::cout << "Waiting..." << std::flush;
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << "Done!\nResults are: "
              << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
    t.join();

    std::vector<double> v = {1.1,2.2,3.3,4.4,5.5,6.6};
    double a = comp2(v);
    std::cout << "sum= " << a  << std::endl;
    std::vector<double> vv;
    for (int i=1; i < 20000; i++)
    {
        double dv = static_cast<double>(i)+static_cast<double>(i)*0.1;
        vv.push_back(dv);
    }
    a = comp4(vv);
    std::cout << "sum= " << a  << std::endl;

}
