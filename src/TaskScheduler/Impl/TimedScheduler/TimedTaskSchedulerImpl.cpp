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

int TimedTaskSchedulerImpl::addTask(std::function<void()> task,void* timeout_int) {
    int timeout = *reinterpret_cast<int*>(timeout_int);
    auto id = getNewTaskId();
    auto expireTime =
        std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
    auto [pt, success] = this->nodeRegestry.emplace(
        std::make_pair(id, std::make_unique<TimerNode>(
                               id, expireTime, task)));
    pushHeap(pt->second.get());
    return id;
}

int TimedTaskSchedulerImpl::adjustTask(int id,void* timeout_int) {
  int timeout = *reinterpret_cast<int*>(timeout_int);
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

void TimedTaskSchedulerImpl::tick() {
  while (!this->timerHeap.empty()) {
    auto node = this->timerHeap.front();
    if ((std::chrono::system_clock::now() - node->expireTime).count() < 0) {
      break;
    }
    node->callBack();
    popHeap();
    this->nodeRegestry.erase(node->id);
  }
}

int TimedTaskSchedulerImpl::getNextTickInterval() {
  tick();
  int interval = 0;
  if (!this->timerHeap.empty()) {
    interval = std::chrono::duration_cast<std::chrono::milliseconds>(
                   this->timerHeap.front()->expireTime -
                   std::chrono::system_clock::now())
                   .count();
    interval = interval < 0 ? 0 : interval;
  }
  return interval;
}

int TimedTaskSchedulerImpl::getTaskCount() { return this->timerHeap.size(); }

int TimedTaskSchedulerImpl::try_excute() {
  while (!this->timerHeap.empty()) {
    auto node = this->timerHeap.front();
    if ((std::chrono::system_clock::now() - node->expireTime).count() < 0) {
      return -1;
    } else {
      node->callBack();
      popHeap();
      this->nodeRegestry.erase(node->id);
      return node->id;
    }
  }
  return -1;
}

int TimedTaskSchedulerImpl::excute(int taskId) {
  auto pt = this->nodeRegestry.find(taskId);
  if (pt == this->nodeRegestry.end()) {
    return -1;
  }
  for (auto i = 0; i < this->timerHeap.size(); i++) {
    if (this->timerHeap[i]->id == taskId) {
      this->timerHeap[i]->callBack();
      break;
    }
  }
  this->timerHeap.back()->callBack();
  return taskId;
}