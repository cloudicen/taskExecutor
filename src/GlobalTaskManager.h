#ifndef __GLOBAL_TASK_MANAGER_H__
#define __GLOBAL_TASK_MANAGER_H__

#include "TaskScheduler/TaskSchedulerBase.h"
#include "ThreadPool/ThreadPool.h"

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <set>
#include <thread>

class GlobalTaskManager {
private:
  GlobalTaskManager() = default;

  void polling();

public:
  static GlobalTaskManager *getInstance();

  ~GlobalTaskManager() { stopExcutor(); };

  static void stopExcutor();

  static void addScheduler(TaskSchedulerBase *);

  static void removeScheduler(TaskSchedulerBase *);

  static void schedulerOnNewTask(TaskSchedulerBase *);

private:
  bool stopFlag = false;
  int pollingInterval = 0;
  std::set<TaskSchedulerBase *> schedulerPolingList;
  std::set<TaskSchedulerBase *> schedulerWaitingList;

  std::thread *schedulerThread;
  std::mutex mtx_;
  std::condition_variable cv_;

  static std::once_flag constructFlag;

private:
  static GlobalTaskManager *instance;

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