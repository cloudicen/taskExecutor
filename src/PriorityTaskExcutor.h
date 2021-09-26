#ifndef __PRIORITY_TASK_EXCUTOR_H__
#define __PRIORITY_TASK_EXCUTOR_H__

#include "GlobalTaskManager.h"
#include "TaskScheduler/PriorityScheduler/PriorityTaskSchedulerProxy.h"

#include <functional>
#include <future>
#include <memory>
#include <thread>

class PriorityTaskExcutor {
public:
  PriorityTaskExcutor() : scheduler(new PriorityTaskSchedulerProxy()) {
    GlobalTaskManager::addScheduler(this->scheduler->getSchedulerBase());
  };

  ~PriorityTaskExcutor() {
    GlobalTaskManager::removeScheduler(this->scheduler->getSchedulerBase());
  };

  template <typename F, typename... Args>
  auto addTaskWithFuture(int priority, F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id =
        this->scheduler->addTask([taskPtr]() { (*taskPtr)(); }, priority);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(
        this->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }

  template <typename F, typename... Args>
  auto addTask(int priority, F &&f, Args &&... args) -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = this->scheduler->addTask(task, priority);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(
        this->scheduler->getSchedulerBase());
    return id;
  }

  int adjustTask(int id, int priority) {
    return scheduler->adjustTask(id, priority);
  }

  int removeTask(int id, bool doCall) {
    return scheduler->removeTask(id, doCall);
  }

  void clearTask() { scheduler->clearTask(); }

private:
  std::unique_ptr<PriorityTaskSchedulerProxy> scheduler;
};

#endif