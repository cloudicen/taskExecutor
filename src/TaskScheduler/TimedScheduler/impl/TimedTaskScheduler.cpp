#include "TimedTaskScheduler.h"

void TimedTaskScheduler::maintainHeap() {
  std::make_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimedTaskNode *a, const TimedTaskNode *b) { return (*a) > (*b); });
}

void TimedTaskScheduler::popHeap() {
  std::pop_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimedTaskNode *a, const TimedTaskNode *b) { return (*a) > (*b); });
  this->timerHeap.pop_back();
}

void TimedTaskScheduler::pushHeap(const TimedTaskNode *node) {
  this->timerHeap.push_back(node);
  std::push_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimedTaskNode *a, const TimedTaskNode *b) { return (*a) > (*b); });
}

int TimedTaskScheduler::addTask(std::function<void()> task, void *timeout_int) {
  std::scoped_lock lk(this->mutex);
  int timeout = *reinterpret_cast<int *>(timeout_int);
  auto id = getNewTaskId();
  auto expireTime =
      std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
  auto [pt, success] = this->nodeRegestry.emplace(
      std::make_pair(id, std::make_unique<TimedTaskNode>(id, expireTime, task)));
  pushHeap(pt->second.get());
  return id;
}

int TimedTaskScheduler::adjustTask(int id, void *timeout_int) {
  std::scoped_lock lk(this->mutex);
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

int TimedTaskScheduler::removeTask(int id, bool doCall) {
  std::scoped_lock lk(this->mutex);
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

void TimedTaskScheduler::clearTask() {
  std::scoped_lock lk(this->mutex);
  timerHeap.clear();
  nodeRegestry.clear();
  resetTaskId();
}

int TimedTaskScheduler::getTaskCount() { 
  std::shared_lock lk(this->mutex);
  return this->timerHeap.size();
   }

std::pair<std::vector<std::function<void()>>, int>
TimedTaskScheduler::getReadyTask() {
  std::scoped_lock lk(this->mutex);
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