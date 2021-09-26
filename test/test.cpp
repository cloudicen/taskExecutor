#include "../src/PriorityTaskExcutor.h"
#include "../src/TimedTaskExcutor.h"

#include <iostream>
#include <thread>
using namespace std::chrono_literals;

int callBack(int i, int *result) {
  std::cout << "call back! No: " << i << std::endl;
  *result = *result + 1;
  return i;
}

int callBackP(int i, int priority) {
  std::cout << "call back! No: " << i << " Priority:" << priority << std::endl;
  std::this_thread::sleep_for(1000ms);
  std::cout << "No: " << i << " Priority:" << priority << " Done!" << std::endl;
  return i;
}

int main() {
  int i = 0;
  int result = 0;
  TimedTaskExcutor *excutor = TimedTaskExcutor::getInstance();
  auto a = excutor->addTaskWithFuture(5000, callBack, i++, &result);
  auto b = excutor->addTaskWithFuture(14000, callBack, i++, &result);
  auto c = excutor->addTaskWithFuture(7000, callBack, i++, &result);
  auto id = excutor->addTaskWithFuture(10000, ::callBack, i++, &result);
  PriorityTaskExcutor excutorP;

  excutorP.addTaskWithFuture(9,callBackP,i++,9);
  auto p = excutorP.addTaskWithFuture(25,callBackP,i++,25);
  excutorP.addTaskWithFuture(3,callBackP,i++,3);
  excutorP.addTaskWithFuture(1,callBackP,i++,1);

  excutorP.adjustTask(p.first,24);

  excutor->adjustTask(id.first, -5000);

  std::cout << id.second.get() << std::endl;
  std::cout << a.second.get() << std::endl;
  auto pp = excutor->addTaskWithFuture(5000, callBack, i++, &result);
  GlobalTaskManager::stopExcutor();
  std::cout << result << std::endl;
}