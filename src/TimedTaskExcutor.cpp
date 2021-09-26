#include "TimedTaskExcutor.h"

TimedTaskExcutor *TimedTaskExcutor::instance;
std::once_flag TimedTaskExcutor::constructFlag;

TimedTaskExcutor *TimedTaskExcutor::getInstance() {
  if (TimedTaskExcutor::instance != nullptr) {
    return instance;
  }
  std::call_once(constructFlag,[](){
      instance = new TimedTaskExcutor();
  });
  return instance;
}