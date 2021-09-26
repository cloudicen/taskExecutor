#include "ThreadPool.h"

std::once_flag ThreadPool::threadPoolConstruct;

ThreadPool ThreadPool::*instance;

ThreadPool *ThreadPool::getInstance(size_t threadNum) {
  if(instance != nullptr){
    return instance;
  }
  std::call_once(ThreadPool::threadPoolConstruct, [threadNum]() {
    for (size_t i = 0; i < threadNum; ++i) {
      instance->workers_.emplace_back([]() {
        for (;;) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> ul(instance->mtx_);
            instance->cv_.wait(ul, []() {
              return instance->stop_ || !instance->tasks_.empty();
            });
            if (instance->stop_ && instance->tasks_.empty()) {
              return;
            }
            task = std::move(instance->tasks_.front());
            instance->tasks_.pop();
          }
          task();
        }
      });
    }
  });
  return instance;
}

void ThreadPool::joinAll() {
  auto instance = ThreadPool::getInstance();
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    instance->stop_ = true;
  }
  instance->cv_.notify_all();
  for (auto &worker : instance->workers_) {
    worker.join();
  }
}

void ThreadPool::submit(std::function<void()> task) {
  auto instance = ThreadPool::getInstance();
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    if (instance->stop_) {
      throw std::runtime_error("submit on stopped ThreadPool");
    }
    instance->tasks_.emplace(task);
  }
  instance->cv_.notify_one();
}