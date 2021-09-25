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
  // timer.adjustTask(id.first, -1000);

  // auto pool = ThreadPool::getInstance();
  // pool->submit([&timer]() {
  //   while (timer.getTaskCount() > 0) {
  //     timer.getNextTickInterval();
  //     // std::cout << "next task timeout: " << timer.getNextTickInterval() <<
  //     // std::endl;
  //   }});
  // TaskExcutor::stopExcutor();
  // while (1) {
  //   using namespace std::chrono_literals;
  //   std::this_thread::sleep_for(20000ms);
  //   break;
  // }
  std::cout << id.second.get() << std::endl;
  std::cout << a.second.get() << std::endl;

  auto pp = excutor.addTaskWithFuture(5000, callBack, i++, &result);
  // while (1) {
  //   using namespace std::chrono_literals;
  //   std::this_thread::sleep_for(20000ms);
  //   break;
  // }

  TaskExcutor::stopExcutor();
  ThreadPool::joinAll();
}