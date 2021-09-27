#include "BasicTaskExecutor.h"

BasicTaskExecutor *BasicTaskExecutor::instance;
std::once_flag BasicTaskExecutor::constructFlag;

BasicTaskExecutor *BasicTaskExecutor::getInstance() {
  if (BasicTaskExecutor::instance != nullptr) {
    return instance;
  }
  std::call_once(constructFlag,[](){
      instance = new BasicTaskExecutor();
  });
  return instance;
}