#ifndef __TASK_SCHEDULER_BASE_H__
#define __TASK_SCHEDULER_BASE_H__

#include <functional>
#include <future>
#include <memory>
#include <queue>

class TaskSchedulerBase {
protected:
  int currentTaskId = 0;
  std::priority_queue<int> reusedId;

  int getNewTaskId() {
    int id = 0;
    if (this->reusedId.empty()) {
      id = currentTaskId++;
    } else {
      id = this->reusedId.top();
      this->reusedId.pop();
    }
    return id;
  }

  void resetTaskId() {
    currentTaskId = 0;
    reusedId = std::priority_queue<int>();
  }

public:
  TaskSchedulerBase() = default;
  virtual ~TaskSchedulerBase() = default;

  virtual int addTask(std::function<void()>&&,
                      std::initializer_list<void *> options) = 0;

  virtual int removeTask(int taskId, bool doCall) = 0;

  virtual int adjustTask(int taskId, std::initializer_list<void *> options) = 0;

  virtual void clearTask() = 0;

  virtual int getTaskCount() = 0;

  virtual std::pair<std::vector<std::function<void()>>, int> getReadyTask() = 0;
};

#endif