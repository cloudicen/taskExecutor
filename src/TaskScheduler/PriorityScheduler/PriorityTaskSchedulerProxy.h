#ifndef __PRIORITY_TASK_SCHEDULER_PROXY_H__
#define __PRIORITY_TASK_SCHEDULER_PROXY_H__

#include "impl/PriorityTaskScheduler.h"
#include <memory>

class PriorityTaskSchedulerProxy {
private:
  std::unique_ptr<PriorityTaskScheduler> PrioritySchedulerBase;

public:
  PriorityTaskSchedulerProxy() : PrioritySchedulerBase(new PriorityTaskScheduler()){};
  ~PriorityTaskSchedulerProxy() = default;

  int addTask(std::function<void()> task, int priority) {
    return PrioritySchedulerBase->addTask(task, &priority);
  }

  int adjustTask(int id, int priority) {
    return PrioritySchedulerBase->adjustTask(id, &priority);
  }

  int removeTask(int id, bool doCall) {
    return PrioritySchedulerBase->removeTask(id, doCall);
  }

  void clearTask() { PrioritySchedulerBase->clearTask(); }

  int getTaskCount() { return PrioritySchedulerBase->getTaskCount(); }

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() {
    return PrioritySchedulerBase->getReadyTask();
  }

  TaskSchedulerBase *getSchedulerBase() { return PrioritySchedulerBase.get(); }
};

#endif