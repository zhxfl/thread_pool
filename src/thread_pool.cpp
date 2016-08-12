// by zhxfl 2016.08.12
#include "thread_pool.h"

namespace iml{
namespace train{

void work_fun(ThreadPool* thread_pool){
    while(true){
        WorkData *work_data = thread_pool->get_work_data();
        //the "_finish" signal is mark and stop the thread
        if(work_data == NULL)
            return;
        work_data->run_work_cb();
        work_data->run_after_work();
    }
}

}}
