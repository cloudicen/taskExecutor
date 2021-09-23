#include "../src/HeapTimer.h"

#include <iostream>
#include <thread>

int callBack(int i, int *result) {
  std::cout << "call back! No: " << i << std::endl;
  *result = *result + 1;
  return i;
}

int main() {
  HeapTimer timer;
  int i = 0;
  int result = 0;
  auto a = timer.add(5000, callBack, i++, &result);
  auto b = timer.add(14000, callBack, i++, &result);
  auto c = timer.add(7000, callBack, i++, &result);
  auto id = timer.add(10000, ::callBack, i++, &result);
  timer.adjust(id.first, -1000);

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