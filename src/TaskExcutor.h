#ifndef __TASK_EXCUTOR__
#define __TASK_EXCUTOR__

#include "ThreadPool/ThreadPool.h"
#include "TaskScheduler/Impl/TaskSchedulerImplBase.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <list>
#include <thread>
#include <chrono>

class TaskExcutor
{
private:
    ThreadPool * pool;
    std::list<TaskSchedulerImplBase*> schedulerPolingList;
    std::list<TaskSchedulerImplBase*> schedulerWaitingList;
    std::thread* schedulerThread;
    std::mutex mtx_;
    std::condition_variable cv_;
    int minPollingInterval = INT_MAX;
    static std::once_flag constructFlag;
    TaskExcutor():pool(ThreadPool::getInstance()){};
    void polling();
public:
    TaskExcutor* getInstance() {
        static TaskExcutor* insPtr = new TaskExcutor();
        auto instance = insPtr; 
        std::call_once(this->constructFlag,[instance](){
            instance->schedulerThread = new std::thread([instance](){
                instance->polling();
            });
            instance->schedulerThread->join();
        });
    }

    ~TaskExcutor();

    void addScheduler(TaskSchedulerImplBase*);
};

#endif