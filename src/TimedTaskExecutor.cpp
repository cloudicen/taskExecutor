#include "TimedTaskExecutor.h"

TimedTaskExecutor *TimedTaskExecutor::instance;
std::once_flag TimedTaskExecutor::constructFlag;

TimedTaskExecutor *TimedTaskExecutor::getInstance() {
  if (TimedTaskExecutor::instance != nullptr) {
    return instance;
  }
  std::call_once(constructFlag,[](){
      instance = new TimedTaskExecutor();
  });
  return instance;
}