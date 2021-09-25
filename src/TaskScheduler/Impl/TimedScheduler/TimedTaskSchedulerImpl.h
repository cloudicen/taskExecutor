#ifndef __HEAP_TIMER_H__
#define __HEAP_TIMER_H__

#include <chrono>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../TaskSchedulerImplBase.h"

class TimedTaskSchedulerImpl;

class TimerNode {
private:
  int id;
  std::chrono::time_point<std::chrono::system_clock> expireTime;
  std::function<void()> callBack;
  friend TimedTaskSchedulerImpl;

public:
  explicit TimerNode(
      int _id, std::chrono::time_point<std::chrono::system_clock> _expireTime,
      std::function<void()> _callBack)
      : id(_id), expireTime(_expireTime), callBack(_callBack){};
  ~TimerNode() = default;

  bool operator>(const TimerNode &other) const {
    return this->expireTime > other.expireTime;
  }
};

class TimedTaskSchedulerImpl : public TaskSchedulerImplBase {
private:
  std::vector<const TimerNode *> timerHeap;
  std::unordered_map<int, std::unique_ptr<TimerNode>> nodeRegestry;

  void maintainHeap();
  void pushHeap(const TimerNode *node);
  void popHeap();

public:
  TimedTaskSchedulerImpl() = default;
  ~TimedTaskSchedulerImpl() = default;
  TimedTaskSchedulerImpl(const TimedTaskSchedulerImpl &) = delete;

  int addTask(std::function<void()> Task, void *timeout_int) override;

  int adjustTask(int id, void *timeout_int) override;

  int removeTask(int id, bool doCall) override;

  void clearTask() override;

  int getTaskCount() override;

  std::pair<std::vector<std::function<void()>>, int> getReadyTask() override;
};

#endif
