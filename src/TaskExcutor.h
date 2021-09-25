#ifndef __TASK_EXCUTOR__
#define __TASK_EXCUTOR__

#include "TaskScheduler/Impl/TaskSchedulerImplBase.h"
#include "ThreadPool/ThreadPool.h"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

class TaskExcutor {
private:
  ThreadPool *pool;
  std::list<TaskSchedulerImplBase *> schedulerPolingList;
  std::list<TaskSchedulerImplBase *> schedulerWaitingList;

  std::thread *schedulerThread;
  bool stop = false;

  static std::once_flag constructFlag;

  std::mutex mtx_;
  std::condition_variable cv_;

  int minPollingInterval = 0;

  TaskExcutor() : pool(ThreadPool::getInstance()){};
  void polling();

public:
  static TaskExcutor *getInstance() {
    static TaskExcutor *insPtr = new TaskExcutor();
    auto instance = insPtr;
    std::call_once(TaskExcutor::constructFlag, [instance]() {
      instance->schedulerThread =
          new std::thread([instance]() { instance->polling(); });
    });
    return instance;
  }

  ~TaskExcutor(){};

  static void stopExcutor() {
    auto instance = TaskExcutor::getInstance();
    {
      std::unique_lock<std::mutex> ul(instance->mtx_);
      instance->stop = true;
    }
    instance->cv_.notify_all();
    instance->schedulerThread->join();
  }

  void addScheduler(TaskSchedulerImplBase *);

  void removeScheduler(TaskSchedulerImplBase *);

  void schedulerOnNewTask(TaskSchedulerImplBase *);
};

#endif