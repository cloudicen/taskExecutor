#include "BasicTaskScheduler.h"

int BasicTaskScheduler::addTask(std::function<void()> task, std::initializer_list<void*> options) {
  std::scoped_lock lk(this->mutex);
  this->taskList.emplace_back(task);
  return 0;
}

void BasicTaskScheduler::clearTask() {
  std::scoped_lock lk(this->mutex);
  taskList.clear();
}

int BasicTaskScheduler::getTaskCount() { 
  std::shared_lock lk(this->mutex);
  return this->taskList.size();
}

std::pair<std::vector<std::function<void()>>, int>
BasicTaskScheduler::getReadyTask() {
  std::scoped_lock lk(this->mutex);
  return {std::move(this->taskList), 0};
}