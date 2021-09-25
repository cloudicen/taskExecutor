#include "TimedTaskSchedulerImpl.h"

void TimedTaskSchedulerImpl::maintainHeap() {
  std::make_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimerNode *a, const TimerNode *b) { return (*a) > (*b); });
}

void TimedTaskSchedulerImpl::popHeap() {
  std::pop_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimerNode *a, const TimerNode *b) { return (*a) > (*b); });
  this->timerHeap.pop_back();
}

void TimedTaskSchedulerImpl::pushHeap(const TimerNode *node) {
  this->timerHeap.push_back(node);
  std::push_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimerNode *a, const TimerNode *b) { return (*a) > (*b); });
}

int TimedTaskSchedulerImpl::addTask(std::function<void()> task,
                                    void *timeout_int) {
  int timeout = *reinterpret_cast<int *>(timeout_int);
  auto id = getNewTaskId();
  auto expireTime =
      std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
  auto [pt, success] = this->nodeRegestry.emplace(
      std::make_pair(id, std::make_unique<TimerNode>(id, expireTime, task)));
  pushHeap(pt->second.get());
  return id;
}

int TimedTaskSchedulerImpl::adjustTask(int id, void *timeout_int) {
  int timeout = *reinterpret_cast<int *>(timeout_int);
  auto pt = this->nodeRegestry.find(id);
  if (pt == this->nodeRegestry.end()) {
    return -1;
  }

  pt->second->expireTime =
      pt->second->expireTime + std::chrono::milliseconds(timeout);
  maintainHeap();
  return id;
}

int TimedTaskSchedulerImpl::removeTask(int id, bool doCall) {
  auto pt = this->nodeRegestry.find(id);
  if (pt == this->nodeRegestry.end()) {
    return -1;
  }

  for (auto i = 0; i < this->timerHeap.size(); i++) {
    if (this->timerHeap[i]->id == id) {
      auto temp = this->timerHeap.back();
      this->timerHeap.back() = this->timerHeap[i];
      this->timerHeap[i] = temp;
      break;
    }
  }
  if (doCall) {
    this->timerHeap.back()->callBack();
  }
  this->timerHeap.pop_back();
  maintainHeap();
  this->nodeRegestry.erase(id);
  return id;
}

void TimedTaskSchedulerImpl::clearTask() {
  timerHeap.clear();
  nodeRegestry.clear();
  resetTaskId();
}

int TimedTaskSchedulerImpl::getTaskCount() { return this->timerHeap.size(); }

std::pair<std::vector<std::function<void()>>, int>
TimedTaskSchedulerImpl::getReadyTask() {
  std::vector<std::function<void()>> taskList;
  int interval = 0;
  while (!this->timerHeap.empty()) {
    auto node = this->timerHeap.front();
    if ((std::chrono::system_clock::now() - node->expireTime).count() < 0) {
      interval = std::chrono::duration_cast<std::chrono::milliseconds>(
                     node->expireTime - std::chrono::system_clock::now())
                     .count();
      interval = interval < 0 ? 0 : interval;
      break;
    } else {
      taskList.push_back(node->callBack);
      popHeap();
      this->nodeRegestry.erase(node->id);
    }
  }
  return {taskList, interval};
}