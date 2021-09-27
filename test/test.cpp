#include "../src/TimedTaskExecutor.h"

#include <iostream>
#include <thread>
using namespace std::chrono_literals;

int callBack(int i, int *result) {
  std::cout << "call back! No: " << i << std::endl;
  *result = *result + 1;
  return i;
}

int main() {
  int i = 0;
  int result = 0;
  TimedTaskExecutor *executor = TimedTaskExecutor::getInstance();
  auto a = executor->addTaskWithFuture(5000, callBack, i++, &result);
  auto b = executor->addTaskWithFuture(14000, callBack, i++, &result);
  auto c = executor->addTaskWithFuture(7000, callBack, i++, &result);
  auto id = executor->addTaskWithFuture(10000, ::callBack, i++, &result);

  executor->adjustTask(id.first, -5000);

  std::cout << id.second.get() << std::endl;
  std::cout << a.second.get() << std::endl;
  auto pp = executor->addTaskWithFuture(5000, callBack, i++, &result);
  GlobalTaskManager::stopService();
  std::cout << result << std::endl;
}