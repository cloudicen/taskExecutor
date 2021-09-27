#ifndef __BASIC_TASK_EXECUTOR_H__
#define __BASIC_TASK_EXECUTOR_H__

#include "GlobalTaskManager.h"
#include "TaskScheduler/BasicScheduler/BasicTaskSchedulerProxy.h"

#include <functional>
#include <future>
#include <memory>
#include <thread>

class BasicTaskExecutor {
private:
  BasicTaskExecutor() : scheduler(new BasicTaskSchedulerProxy()) {
    GlobalTaskManager::addScheduler(this->scheduler->getSchedulerBase());
  };
public:
  static BasicTaskExecutor* getInstance();
  ~BasicTaskExecutor() {
    GlobalTaskManager::removeScheduler(this->scheduler->getSchedulerBase());
  };

  template <typename F, typename... Args>
  static auto addTaskWithFuture(F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id = instance->scheduler->addTask([taskPtr]() { (*taskPtr)(); });
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(instance->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }

  template <typename F, typename... Args>
  static auto addTask(F &&f, Args &&... args) -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = instance->scheduler->addTask(task);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(instance->scheduler->getSchedulerBase());
    return id;
  }

  void clearTask() { scheduler->clearTask(); }

private:
  std::unique_ptr<BasicTaskSchedulerProxy> scheduler;
  static std::once_flag constructFlag;
  static BasicTaskExecutor* instance;

private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (BasicTaskExecutor::instance != NULL) {
        delete BasicTaskExecutor::instance;
        BasicTaskExecutor::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif