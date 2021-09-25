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
  static GlobalTaskManager *instance;
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
  static GlobalTaskManager *getInstance();

  ~GlobalTaskManager() { stopExcutor(); };

  void stopExcutor() {
    {
      std::unique_lock<std::mutex> ul(this->mtx_);
      this->stop = true;
    }
    this->cv_.notify_all();
    this->schedulerThread->join();
  }

  void addScheduler(TaskSchedulerBase *);

  void removeScheduler(TaskSchedulerBase *);

  void schedulerOnNewTask(TaskSchedulerBase *);

private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (GlobalTaskManager::instance != NULL) {
        delete GlobalTaskManager::instance;
        GlobalTaskManager::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif