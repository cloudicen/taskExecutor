#include "PriorityTaskScheduler.h"

void PriorityTaskScheduler::maintainHeap() {
  std::make_heap(
      this->priorityHeap.begin(), this->priorityHeap.end(),
      [](const PriorityTaskNode *a, const PriorityTaskNode *b) { return (*a) > (*b); });
}

void PriorityTaskScheduler::popHeap() {
  std::pop_heap(
      this->priorityHeap.begin(), this->priorityHeap.end(),
      [](const PriorityTaskNode *a, const PriorityTaskNode *b) { return (*a) > (*b); });
  this->priorityHeap.pop_back();
}

void PriorityTaskScheduler::pushHeap(const PriorityTaskNode *node) {
  this->priorityHeap.push_back(node);
  std::push_heap(
      this->priorityHeap.begin(), this->priorityHeap.end(),
      [](const PriorityTaskNode *a, const PriorityTaskNode *b) { return (*a) > (*b); });
}

int PriorityTaskScheduler::addTask(std::function<void()> task, void *priority_int) {
  std::scoped_lock lk(this->mutex);
  int priority = *reinterpret_cast<int *>(priority_int);
  auto id = getNewTaskId();
  auto [pt, success] = this->nodeRegestry.emplace(
      std::make_pair(id, std::make_unique<PriorityTaskNode>(id, priority, task)));
  pushHeap(pt->second.get());
  return id;
}

int PriorityTaskScheduler::adjustTask(int id, void *priority_int) {
  std::scoped_lock lk(this->mutex);
  int priority = *reinterpret_cast<int *>(priority_int);
  auto pt = this->nodeRegestry.find(id);
  if (pt == this->nodeRegestry.end()) {
    return -1;
  }

  pt->second->priority = priority;
  maintainHeap();
  return id;
}

int PriorityTaskScheduler::removeTask(int id, bool doCall) {
  std::scoped_lock lk(this->mutex);
  auto pt = this->nodeRegestry.find(id);
  if (pt == this->nodeRegestry.end()) {
    return -1;
  }

  for (auto i = 0; i < this->priorityHeap.size(); i++) {
    if (this->priorityHeap[i]->id == id) {
      auto temp = this->priorityHeap.back();
      this->priorityHeap.back() = this->priorityHeap[i];
      this->priorityHeap[i] = temp;
      break;
    }
  }
  if (doCall) {
    this->priorityHeap.back()->callBack();
  }
  this->priorityHeap.pop_back();
  maintainHeap();
  this->nodeRegestry.erase(id);
  return id;
}

void PriorityTaskScheduler::clearTask() {
  std::scoped_lock lk(this->mutex);
  priorityHeap.clear();
  nodeRegestry.clear();
  resetTaskId();
}

int PriorityTaskScheduler::getTaskCount() { 
  std::shared_lock lk(this->mutex);
  return this->priorityHeap.size();
   }

std::pair<std::vector<std::function<void()>>, int>
PriorityTaskScheduler::getReadyTask() {
  std::scoped_lock lk(this->mutex);
  std::vector<std::function<void()>> taskList;
  for (auto taskNode : priorityHeap) {
    taskList.push_back(taskNode->callBack);
  }
  priorityHeap.clear();
  nodeRegestry.clear();
  resetTaskId();
  return {taskList, 0};
}