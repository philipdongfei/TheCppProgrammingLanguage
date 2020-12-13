#include <thread>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

void f(const vector<double>& v, double* res)
{
    *res = 0;
    for (double n : v){
        if (n > *res)
            *res = n;
    }
    
}

class F {
public:
    F(const vector<double>& vv, double* p):v{vv},res{p}{}
    void operator()(){
        //vector<double>::iterator result = min_element(v.begin(),v.end());
        //*res = *result;
        *res = 0;
        for(double n : v)
        {
            if (n > *res)
                *res = n;
        }
    }

private:
    const vector<double>& v;
    double* res;
};

int main()
{
    vector<double> some_vec;
    vector<double> vec2;

    vector<double>::iterator it,it2;
    it = some_vec.begin();
    it2 = vec2.begin();
    for (int i=0; i<10; i++)
    {
        it = some_vec.insert(it,i*0.1);
        it2 = vec2.insert(it2,i*0.0021);
    }

    double res1;
    double res2;

    thread t1{f, some_vec,&res1};
    thread t2{F{vec2,&res2}};

    t1.join();
    t2.join();

    cout << res1 << ' ' << res2 << '\n';

    return 0;
}
