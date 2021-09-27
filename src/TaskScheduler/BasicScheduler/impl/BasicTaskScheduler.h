#ifndef __BASIC_TASK_SCHEDULER_H__
#define __BASIC_TASK_SCHEDULER_H__

#include <functional>
#include <vector>
#include <shared_mutex>
#include <mutex>

#include "../../TaskSchedulerBase.h"

class BasicTaskScheduler : public TaskSchedulerBase {
public:
  BasicTaskScheduler() = default;
  ~BasicTaskScheduler() = default;
  BasicTaskScheduler(const BasicTaskScheduler &) = delete;

  int addTask(std::function<void()>&& Task, std::initializer_list<void*> options = {}) override;

  int adjustTask(int id, std::initializer_list<void*> options = {}) override{return 0;};

  int removeTask(int id, bool doCall) override{return 0;};

  void clearTask() override;

  int getTaskCount() override;

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() override;

private:
  std::vector<std::function<void()>> taskList;
  std::shared_mutex mutex;
};

#endif