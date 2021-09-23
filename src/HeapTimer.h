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

class HeapTimer;

class TimerNode {
private:
  int id;
  std::chrono::time_point<std::chrono::system_clock> expireTime;
  std::function<void()> callBack;
  friend HeapTimer;

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

class HeapTimer {
private:
  std::vector<const TimerNode *> timerHeap;
  std::unordered_map<int, std::unique_ptr<TimerNode>> nodeRegestry;
  int currentNodeId = 0;
  std::priority_queue<int> reusedId;

  void maintainHeap();
  void pushHeap(const TimerNode *node);
  void popHeap();

public:
  HeapTimer() = default;
  ~HeapTimer() = default;
  HeapTimer(const HeapTimer &) = delete;

  template <typename F, typename... Args>
  auto add(int timeout, F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id = 0;
    if (this->reusedId.empty()) {
      id = currentNodeId++;
    } else {
      id = this->reusedId.top();
      this->reusedId.pop();
    }
    auto expireTime =
        std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
    auto [pt, success] = this->nodeRegestry.emplace(
        std::make_pair(id, std::make_unique<TimerNode>(
                               id, expireTime, [taskPtr]() { (*taskPtr)(); })));
    pushHeap(pt->second.get());
    return {id, taskPtr->get_future()};
  }

  int adjust(int id, int timeout);

  int remove(int id, bool doCall);

  void clear();

  void tick();

  int getNextTickInterval();

  int getTaskCount();
};

#endif
