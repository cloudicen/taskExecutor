#ifndef __BASIC_TASK_SCHEDULER_PROXY_H__
#define __BASIC_TASK_SCHEDULER_PROXY_H__

#include "impl/BasicTaskScheduler.h"
#include <memory>

class BasicTaskSchedulerProxy {
private:
  std::unique_ptr<BasicTaskScheduler> BasicSchedulerBase;

public:
  BasicTaskSchedulerProxy() : BasicSchedulerBase(new BasicTaskScheduler()){};
  ~BasicTaskSchedulerProxy() = default;

  int addTask(std::function<void()> task) {
    return BasicSchedulerBase->addTask(task);
  }

  void clearTask() { BasicSchedulerBase->clearTask(); }

  int getTaskCount() { return BasicSchedulerBase->getTaskCount(); }

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() {
    return BasicSchedulerBase->getReadyTask();
  }

  TaskSchedulerBase *getSchedulerBase() { return BasicSchedulerBase.get(); }
};

#endif