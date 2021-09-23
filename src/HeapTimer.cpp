#include "HeapTimer.h"

void HeapTimer::maintainHeap() {
  std::make_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimerNode *a, const TimerNode *b) { return (*a) > (*b); });
}

void HeapTimer::popHeap() {
  std::pop_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimerNode *a, const TimerNode *b) { return (*a) > (*b); });
  this->timerHeap.pop_back();
}

void HeapTimer::pushHeap(const TimerNode *node) {
  this->timerHeap.push_back(node);
  std::push_heap(
      this->timerHeap.begin(), this->timerHeap.end(),
      [](const TimerNode *a, const TimerNode *b) { return (*a) > (*b); });
}

int HeapTimer::adjust(int id, int timeout) {
  auto pt = this->nodeRegestry.find(id);
  if (pt == this->nodeRegestry.end()) {
    return -1;
  }

  pt->second->expireTime =
      pt->second->expireTime + std::chrono::milliseconds(timeout);
  maintainHeap();
  return id;
}

int HeapTimer::remove(int id, bool doCall) {
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

void HeapTimer::clear() {
  timerHeap.clear();
  nodeRegestry.clear();
  currentNodeId = 0;
  reusedId = std::priority_queue<int>();
}

void HeapTimer::tick() {
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

int HeapTimer::getNextTickInterval() {
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

int HeapTimer::getTaskCount() { return this->timerHeap.size(); }