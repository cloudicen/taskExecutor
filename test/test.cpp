#include "../src/TimedTaskExecutor.h"
#include "../src/BasicTaskExecutor.h"

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
  BasicTaskExecutor *b_executor = BasicTaskExecutor::getInstance();
  auto a = executor->addTaskWithFuture(5000, callBack, i++, &result);
  auto b = executor->addTaskWithFuture(14000, callBack, i++, &result);
  auto c = executor->addTaskWithFuture(7000, callBack, i++, &result);
  auto id = executor->addTaskWithFuture(10000, ::callBack, i++, &result);

  executor->adjustTask(id.first, -5000);

  std::cout << id.second.get() << std::endl;
  std::cout << a.second.get() << std::endl;

  auto ff = b_executor->addTaskWithFuture(callBack, i++, &result);
  auto gg = b_executor->addTaskWithFuture(callBack, i++, &result);
  auto hh = b_executor->addTaskWithFuture(callBack, i++, &result);

  std::cout << gg.second.get() << std::endl;
  
  auto pp = executor->addTask(1000,true, callBack, i++, &result);
  GlobalTaskManager::stopService();
  std::cout << result << std::endl;
}