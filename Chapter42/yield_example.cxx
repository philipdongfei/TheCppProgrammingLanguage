#include <iostream>
#include <thread>
#include <atomic>

std::atomic<bool> ready(false);

void countlm(int id){
    while (!ready){ //wait until main() sets ready...
        std::this_thread::yield();
    }
    for (volatile int i=0; i < 1000000; ++i){}
    std::cout << id << ' ';
}

int main()
{
    std::thread threads[10];
    std::cout << "race of 10 threads that count to 1 million:\n";
    for (int i=0; i<10; ++i) threads[i] = std::thread(countlm,i);
    ready = true; //go
    for (auto& th : threads) th.join();
    std::cout << '\n';
    return 0;
}
