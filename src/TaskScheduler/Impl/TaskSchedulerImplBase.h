#ifndef __TASK_SCHEDULER_BASE__
#define __TASK_SCHEDULER_BASE__

#include <functional>
#include <future>
#include <memory>
#include <queue>

class TaskSchedulerImplBase {
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
  TaskSchedulerImplBase() = default;
  virtual ~TaskSchedulerImplBase() = default;

  virtual int addTask(std::function<void()>,void* options) = 0;

  virtual int removeTask(int taskId,bool doCall) = 0;

  virtual int adjustTask(int taskId,void* options) = 0;

  virtual void clearTask() = 0;

  virtual int getTaskCount() = 0;

  virtual int try_excute() = 0;

  virtual int excute(int taskId) = 0;
};

#endif