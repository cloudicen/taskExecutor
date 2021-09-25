#include "../src/TimedTaskExcutor.h"

#include <iostream>
#include <thread>

int callBack(int i, int *result) {
  std::cout << "call back! No: " << i << std::endl;
  *result = *result + 1;
  return i;
}

int main() {
  TimedTaskExcutor excutor;

  // TimedTaskScheduler timer;
  int i = 0;
  int result = 0;
  auto a = excutor.addTaskWithFuture(5000, callBack, i++, &result);
  auto b = excutor.addTaskWithFuture(14000, callBack, i++, &result);
  auto c = excutor.addTaskWithFuture(7000, callBack, i++, &result);
  auto id = excutor.addTaskWithFuture(10000, ::callBack, i++, &result);

  std::cout << id.second.get() << std::endl;
  std::cout << a.second.get() << std::endl;

  auto pp = excutor.addTaskWithFuture(5000, callBack, i++, &result);

  GlobalTaskManager::stopExcutor();
  ThreadPool::joinAll();
}