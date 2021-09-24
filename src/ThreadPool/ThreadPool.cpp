#include "ThreadPool.h"

std::once_flag ThreadPool::threadPoolConstruct;

ThreadPool* ThreadPool::getInstance(size_t threadNum) {
  static ThreadPool* insPtr = new ThreadPool();
  auto instance = insPtr;
  std::call_once(ThreadPool::threadPoolConstruct, [instance,threadNum]() {
    for (size_t i = 0; i < threadNum; ++i) {
      instance->workers_.emplace_back([instance]() {
        for (;;) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> ul(instance->mtx_);
            instance->cv_.wait(ul, [instance]() { return instance->stop_ || !instance->tasks_.empty(); });
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
  {
    std::unique_lock<std::mutex> ul(mtx_);
    if (stop_) {
      throw std::runtime_error("submit on stopped ThreadPool");
    }
    tasks_.emplace(task);
  }
  cv_.notify_one();
}