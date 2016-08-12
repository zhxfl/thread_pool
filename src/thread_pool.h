// zhxfl 2016.08.11
#pragma once
#include <iostream>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>

const int THREAD_POOL_SIZE = 16;

namespace iml{
namespace train{
class ThreadPool;

class Semaphore{
    public:
        Semaphore():_wakeups(0){
        }

        // the thread is suspended util "_wakeups > 0"
        void wait(){
            std::unique_lock<std::mutex>lock{_mutex};
            _condition.wait(lock, [&]()->bool{return _wakeups > 0;});
        }

        // inc "_wakeups and notify all suspend thread to continue
        void signal(){
            std::unique_lock<std::mutex>lock{_mutex};
            _wakeups++;
            _condition.notify_all();
        }

    private:
        int _wakeups;
        std::mutex _mutex;
        std::condition_variable _condition;
};

class WorkData{
    public:
        WorkData(const std::function<void()>& work_cb, const std::function<void()>& after_work){
            set_work_cb(work_cb);
            set_after_work(after_work);
        }

        WorkData(){
            _work_cb = [](){};
            _after_work = [&](){};
        }

        //call back function
        void set_work_cb(const std::function<void()>& work_cb){
            _work_cb = work_cb;
        }

        //after work function
        void set_after_work(const std::function<void()>& after_work){
            _after_work = after_work;
        }
        void wait(){
            _semaphore.wait();
        }
        void run_work_cb(){
            _work_cb();
        }
        void run_after_work(){
            _after_work();
            //finish the work and notify all suspended thread to continue
            _semaphore.signal();
        }
    private:
        std::function<void()>_work_cb;
        std::function<void()>_after_work;
        Semaphore _semaphore;
};

void work_fun(ThreadPool* thread_pool);

class ThreadPool{
    public:
        friend void work_fun(ThreadPool* thread_pool);

        ThreadPool(){
            _finish = false;
            for(size_t i = 0; i < THREAD_POOL_SIZE; i++){
                _thread.push_back(std::thread(work_fun, this));
            }
        }

        ~ThreadPool(){
            _finish = true;
            _condition.notify_all();
            for(size_t i = 0; i < _thread.size(); i++){
                _thread[i].join();
            }
        }

        // the single instance 
        static ThreadPool& instance(){
            static ThreadPool thread_pool;
            return thread_pool;
        }

        WorkData* get_work_data(){
            //get lock to make sure atomic of operators of "_queue"
            std::unique_lock<std::mutex>lck(_mutex);

            //wait util the condition is met
            _condition.wait(lck, [&]()->bool{return _queue.size() > 0 || _finish;});

            if(_finish == true || _queue.size() == 0)
                return NULL;
            WorkData* work_data = _queue.front();
            _queue.pop();
            return work_data;
        }

        void post(WorkData& work_data){
            std::unique_lock<std::mutex>lck(_mutex);
            _queue.push(&work_data);
            //notify all suspended threads to "get_work_data"
            _condition.notify_all();
        }
    private:
        std::vector<std::thread> _thread;
        std::mutex _mutex;
        std::queue<WorkData*>_queue;
        std::condition_variable _condition;
        bool _finish;//the signal to stop all threads
};
}}
