#include "../src/TaskScheduler/TimedTaskScheduler.h"

#include <iostream>
#include <thread>

int callBack(int i, int *result) {
  std::cout << "call back! No: " << i << std::endl;
  *result = *result + 1;
  return i;
}

int main() {
  TimedTaskScheduler timer;
  int i = 0;
  int result = 0;
  auto a = timer.addTaskWithFuture(5000, callBack, i++, &result);
  auto b = timer.addTaskWithFuture(14000, callBack, i++, &result);
  auto c = timer.addTaskWithFuture(7000, callBack, i++, &result);
  auto id = timer.addTaskWithFuture(10000, ::callBack, i++, &result);
  timer.adjustTask(id.first, -1000);

  std::jthread t([&timer] {
    while (timer.getTaskCount() > 0) {
      timer.getNextTickInterval();
      // std::cout << "next task timeout: " << timer.getNextTickInterval() <<
      // std::endl;
    }
  });
  std::cout << id.second.get() << std::endl;
  std::cout << a.second.get() << std::endl;
  if (t.joinable()) {
    t.join();
  }
}