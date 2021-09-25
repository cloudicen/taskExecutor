#ifndef __GLOBAL_TASK_MANAGER_H__
#define __GLOBAL_TASK_MANAGER_H__

#include "TaskScheduler/TaskSchedulerBase.h"
#include "ThreadPool/ThreadPool.h"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

class GlobalTaskManager {
private:
  ThreadPool *pool;
  std::list<TaskSchedulerBase *> schedulerPolingList;
  std::list<TaskSchedulerBase *> schedulerWaitingList;

  std::thread *schedulerThread;
  bool stop = false;

  static std::once_flag constructFlag;

  std::mutex mtx_;
  std::condition_variable cv_;

  int minPollingInterval = 0;

  GlobalTaskManager() : pool(ThreadPool::getInstance()){};
  void polling();

public:
  static GlobalTaskManager *getInstance() {
    static GlobalTaskManager *insPtr = new GlobalTaskManager();
    auto instance = insPtr;
    std::call_once(GlobalTaskManager::constructFlag, [instance]() {
      instance->schedulerThread =
          new std::thread([instance]() { instance->polling(); });
    });
    return instance;
  }

  ~GlobalTaskManager(){};

  static void stopExcutor() {
    auto instance = GlobalTaskManager::getInstance();
    {
      std::unique_lock<std::mutex> ul(instance->mtx_);
      instance->stop = true;
    }
    instance->cv_.notify_all();
    instance->schedulerThread->join();
  }

  void addScheduler(TaskSchedulerBase *);

  void removeScheduler(TaskSchedulerBase *);

  void schedulerOnNewTask(TaskSchedulerBase *);
};

#endif