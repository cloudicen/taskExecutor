#ifndef __TIMED_TASK_EXCUTOR_H__
#define __TIMED_TASK_EXCUTOR_H__

#include "GlobalTaskManager.h"
#include "TaskScheduler/TimedScheduler/TimedTaskSchedulerProxy.h"

#include <functional>
#include <future>
#include <memory>
#include <thread>

class TimedTaskExcutor {
private:
  TimedTaskExcutor() : scheduler(new TimedTaskSchedulerProxy()) {
    GlobalTaskManager::addScheduler(this->scheduler->getSchedulerBase());
  };
public:
  static TimedTaskExcutor* getInstance();
  ~TimedTaskExcutor() {
    GlobalTaskManager::removeScheduler(this->scheduler->getSchedulerBase());
  };

  template <typename F, typename... Args>
  static auto addTaskWithFuture(int timeout, F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id = instance->scheduler->addTask([taskPtr]() { (*taskPtr)(); }, timeout);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(instance->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }

  template <typename F, typename... Args>
  static auto addTask(int timeout, F &&f, Args &&... args) -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = instance->scheduler->addTask(task, timeout);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(instance->scheduler->getSchedulerBase());
    return id;
  }

   int addTask(std::function<void()> task, int timeout) {
    return scheduler->addTask(task, timeout);
  }

  int adjustTask(int id, int timeout) {
    return scheduler->adjustTask(id,timeout);
  }

  int removeTask(int id, bool doCall) {
    return scheduler->removeTask(id, doCall);
  }

  void clearTask() { scheduler->clearTask(); }

private:
  std::unique_ptr<TimedTaskSchedulerProxy> scheduler;
  static std::once_flag constructFlag;
  static TimedTaskExcutor* instance;

private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (TimedTaskExcutor::instance != NULL) {
        delete TimedTaskExcutor::instance;
        TimedTaskExcutor::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif