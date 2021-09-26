#ifndef __TIMED_TASK_SCHEDULER_PROXY_H__
#define __TIMED_TASK_SCHEDULER_PROXY_H__

#include "impl/TimedTaskScheduler.h"
#include <memory>

class TimedTaskSchedulerProxy {
private:
  std::unique_ptr<TimedTaskScheduler> timedSchedulerBase;

public:
  TimedTaskSchedulerProxy() : timedSchedulerBase(new TimedTaskScheduler()){};
  ~TimedTaskSchedulerProxy() = default;

  int addTask(std::function<void()> task, int timeout) {
    return timedSchedulerBase->addTask(task, &timeout);
  }

  int adjustTask(int id, int timeout) {
    return timedSchedulerBase->adjustTask(id, &timeout);
  }

  int removeTask(int id, bool doCall) {
    return timedSchedulerBase->removeTask(id, doCall);
  }

  void clearTask() { timedSchedulerBase->clearTask(); }

  int getTaskCount() { return timedSchedulerBase->getTaskCount(); }

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() {
    return timedSchedulerBase->getReadyTask();
  }

  TaskSchedulerBase *getSchedulerBase() { return timedSchedulerBase.get(); }
};

#endif