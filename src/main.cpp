#include <iostream>
#include <unistd.h>
#include "thread_pool.h"
#include <functional>
using namespace std;
using namespace iml::train;

int fib_(int n){
    int* ret = new int[n + 1];
    ret[0] = 1;
    ret[1] = 1;
    if(n <=1)return ret[n];
    else{
        for(int i = 2; i <= n; i++){
            ret[i] = ret[i - 1] + ret[i - 2];
        }
    }
    return ret[n];
}

int main(){
    WorkData* work_data = new WorkData[100];
    for(int i = 0; i < 100; i++){
        work_data[i].set_work_cb(
                [=](){cout<<i<<" "<<fib_(i % 40)<<endl;}
                );
        work_data[i].set_after_work([](){cout<<"finish"<<endl;});
        ThreadPool::instance().post(work_data[i]);
    }

    for(int i = 0; i < 100; i++){
        work_data[i].wait();
    }

    delete [] work_data;
    return 0;
}
