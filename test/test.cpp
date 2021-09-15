#include "../src/HeapTimer.h"

#include <iostream>

void callBack(int i) {
    std::cout << "call back! No: " << i << std::endl;
}

int main() {
    HeapTimer timer;
    int i = 0;
    timer.add(5000,std::bind(callBack,i++));
    timer.add(14000,std::bind(callBack,i++));
    timer.add(7000,std::bind(callBack,i++));
    auto id = timer.add(10000,std::bind(callBack,i++));
    timer.adjust(id,-1000);
    while (timer.getTaskCount() > 0)
    {
        std::cout << "next task timeout: " << timer.getNextTickInterval() << std::endl;
    }
}

