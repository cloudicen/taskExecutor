#ifndef __TIMED_TASK_SCHEDULER_H__
#define __TIMED_TASK_SCHEDULER_H__

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <assert.h>

#include "../../TaskSchedulerBase.h"

class TimedTaskScheduler;

class TimedTaskNode {
public:
  explicit TimedTaskNode(
      int _id, std::chrono::time_point<std::chrono::system_clock> _expireTime,
      std::function<void()> &&_callBack,int _interval,bool _repeat)
      : id(_id), expireTime(_expireTime), callBack(_callBack),interval(_interval),repeatTask(_repeat){};
  ~TimedTaskNode() = default;

  bool operator>(const TimedTaskNode &other) const {
    return this->expireTime > other.expireTime;
  }

private:
  int id;
  std::chrono::time_point<std::chrono::system_clock> expireTime;
  int interval;
  std::function<void()> callBack;
  bool repeatTask;
  friend TimedTaskScheduler;
};

class TimedTaskScheduler : public TaskSchedulerBase {
private:
  void maintainHeap();
  void pushHeap(TimedTaskNode *node);
  void popHeap();

public:
  TimedTaskScheduler() = default;
  ~TimedTaskScheduler() = default;
  TimedTaskScheduler(const TimedTaskScheduler &) = delete;

  int addTask(std::function<void()>&& Task,
              std::initializer_list<void *> options) override;

  int adjustTask(int id, std::initializer_list<void *> options) override;

  int removeTask(int id, bool doCall) override;

  void clearTask() override;

  int getTaskCount() override;

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() override;

private:
  std::vector<TimedTaskNode *> timerHeap;
  std::unordered_map<int, std::unique_ptr<TimedTaskNode>> nodeRegestry;
  std::shared_mutex mutex;
};

#endif
