#ifndef __PRIORITY_TASK_SCHEDULER_H__
#define __PRIORITY_TASK_SCHEDULER_H__

#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <mutex>

#include "../../TaskSchedulerBase.h"

class PriorityTaskScheduler;

class PriorityTaskNode {
public:
  explicit PriorityTaskNode(
      int _id, int _priority,
      std::function<void()> _callBack)
      : id(_id), priority(_priority), callBack(_callBack){};
  ~PriorityTaskNode() = default;

  bool operator>(const PriorityTaskNode &other) const {
    return this->priority > other.priority;
  }

private:
  int id;
  int priority;
  std::function<void()> callBack;
  friend PriorityTaskScheduler;
};

class PriorityTaskScheduler : public TaskSchedulerBase {
private:
  void maintainHeap();
  void pushHeap(const PriorityTaskNode *node);
  void popHeap();

public:
  PriorityTaskScheduler() = default;
  ~PriorityTaskScheduler() = default;
  PriorityTaskScheduler(const PriorityTaskScheduler &) = delete;

  int addTask(std::function<void()> Task, void *priority_int) override;

  int adjustTask(int id, void *priority_int) override;

  int removeTask(int id, bool doCall) override;

  void clearTask() override;

  int getTaskCount() override;

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() override;

private:
  std::vector<const PriorityTaskNode *> priorityHeap;
  std::unordered_map<int, std::unique_ptr<PriorityTaskNode>> nodeRegestry;
  std::shared_mutex mutex;
};

#endif
