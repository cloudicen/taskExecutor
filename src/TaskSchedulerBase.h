#ifndef __TASK_SCHEDULER_BASE__
#define __TASK_SCHEDULER_BASE__

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

  virtual int try_excute() = 0;

  virtual int excute(int taskId) = 0;
};

#endif