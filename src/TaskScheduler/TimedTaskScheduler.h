#ifndef __TIMED_TASK_SCHEDULER__
#define __TIMED_TASK_SCHEDULER__

#include "Impl/TimedScheduler/TimedTaskSchedulerImpl.h"

class TimedTaskScheduler {
private:
  TimedTaskSchedulerImpl *timer;

public:
  TimedTaskScheduler() : timer(new TimedTaskSchedulerImpl()){};
  ~TimedTaskScheduler() = default;

  // template <typename F, typename... Args>
  // auto addTaskWithFuture(int timeout, F &&f, Args &&... args)
  //     -> std::pair<int, std::future<decltype(f(args...))>> {
  //   auto taskPtr =
  //   std::make_shared<std::packaged_task<decltype(f(args...))()>>(
  //       std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  //   int id = this->timer->addTask([taskPtr]() { (*taskPtr)(); }, &timeout);
  //   return {id, taskPtr->get_future()};
  // }

  // template <typename F, typename... Args>
  // auto addTask(int timeout, F &&f, Args &&... args) -> int {
  //   auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
  //   int id = this->timer->addTask(task, &timeout);
  //   return id;
  // }
  int addTask(std::function<void()> task, int timeout) {
    return timer->addTask(task, &timeout);
  }

  int adjustTask(int id, int timeout) {
    return timer->adjustTask(id, &timeout);
  }

  int removeTask(int id, bool doCall) { return timer->removeTask(id, doCall); }

  void clearTask() { timer->clearTask(); }

  int getTaskCount() { return timer->getTaskCount(); }

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() {
    return timer->getReadyTask();
  }
};

#endif