#ifndef __TIMED_TASK_EXCUTOR__
#define __TIMED_TASK_EXCUTOR__

#include "TaskScheduler/TimedTaskScheduler.h"
#include "ThreadPool/ThreadPool.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class TimedTaskExcutor
{
private:
    ThreadPool * pool;
    TimedTaskScheduler * scheduler;
    std::thread schedulerThread;
public:
    TimedTaskExcutor() = default;
    ~TimedTaskExcutor() = default;
};


#endif