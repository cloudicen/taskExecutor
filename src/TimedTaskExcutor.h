#ifndef __TIMED_TASK_EXCUTOR_H__
#define __TIMED_TASK_EXCUTOR_H__

#include "GlobalTaskManager.h"
#include "TaskScheduler/TimedScheduler/TimedTaskSchedulerProxy.h"
#include "ThreadPool/ThreadPool.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class TimedTaskExcutor {
public:
  TimedTaskExcutor() : scheduler(new TimedTaskSchedulerProxy()) {
    GlobalTaskManager::addScheduler(this->scheduler->getSchedulerBase());
  };
  ~TimedTaskExcutor() {
    GlobalTaskManager::removeScheduler(this->scheduler->getSchedulerBase());
  };

  template <typename F, typename... Args>
  auto addTaskWithFuture(int timeout, F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id = this->scheduler->addTask([taskPtr]() { (*taskPtr)(); }, timeout);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(this->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }

  template <typename F, typename... Args>
  auto addTask(int timeout, F &&f, Args &&... args) -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = this->scheduler->addTask(task, timeout);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(this->scheduler->getSchedulerBase());
    return id;
  }

private:
  std::unique_ptr<TimedTaskSchedulerProxy> scheduler;
  std::thread schedulerThread;
};

#endif