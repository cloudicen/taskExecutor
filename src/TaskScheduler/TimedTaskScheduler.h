#ifndef __TIMED_TASK_SCHEDULER__
#define __TIMED_TASK_SCHEDULER__

#include "Impl/TimedScheduler/TimedTaskSchedulerImpl.h"
#include <memory>

class TimedTaskScheduler {
private:
  std::unique_ptr<TimedTaskSchedulerImpl> timedSchedulerBase;

public:
  TimedTaskScheduler() : timedSchedulerBase(new TimedTaskSchedulerImpl()){};
  ~TimedTaskScheduler() = default;

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

  TaskSchedulerImplBase *getSchedulerBase() { return timedSchedulerBase.get(); }
};

#endif