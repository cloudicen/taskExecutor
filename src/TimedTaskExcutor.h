#ifndef __TIMED_TASK_EXCUTOR__
#define __TIMED_TASK_EXCUTOR__

#include "TaskExcutor.h"
#include "TaskScheduler/TimedTaskScheduler.h"
#include "ThreadPool/ThreadPool.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class TimedTaskExcutor {
private:
  TaskExcutor *excutor;
  std::unique_ptr<TimedTaskScheduler> scheduler;
  std::thread schedulerThread;

public:
  TimedTaskExcutor()
      : excutor(TaskExcutor::getInstance()),
        scheduler(new TimedTaskScheduler()) {
    excutor->addScheduler(this->scheduler->getSchedulerBase());
  };
  ~TimedTaskExcutor() {
    excutor->removeScheduler(this->scheduler->getSchedulerBase());
  };

  template <typename F, typename... Args>
  auto addTaskWithFuture(int timeout, F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id = this->scheduler->addTask([taskPtr]() { (*taskPtr)(); }, timeout);
    //通知执行器，有新任务加入
    excutor->schedulerOnNewTask(this->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }

  template <typename F, typename... Args>
  auto addTask(int timeout, F &&f, Args &&... args) -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = this->scheduler->addTask(task, timeout);
    //通知执行器，有新任务加入
    excutor->schedulerOnNewTask(this->scheduler->getSchedulerBase());
    return id;
  }
};

#endif