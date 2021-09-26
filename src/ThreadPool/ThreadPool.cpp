#include "ThreadPool.h"

std::once_flag ThreadPool::threadPoolConstruct;
ThreadPool *ThreadPool::instance;

ThreadPool *ThreadPool::getInstance(size_t threadNum) {
  if (ThreadPool::instance != nullptr) {
    return ThreadPool::instance;
  }
  std::call_once(ThreadPool::threadPoolConstruct, [threadNum]() {
    ThreadPool::instance = new ThreadPool();
    for (size_t i = 0; i < threadNum; ++i) {
      ThreadPool::instance->workers_.emplace_back([]() {
        for (;;) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> ul(ThreadPool::instance->mtx_);
            ThreadPool::instance->cv_.wait(ul, []() {
              return ThreadPool::instance->stop_ ||
                     !ThreadPool::instance->tasks_.empty();
            });
            if (ThreadPool::instance->stop_ &&
                ThreadPool::instance->tasks_.empty()) {
              return;
            }
            task = std::move(ThreadPool::instance->tasks_.front());
            ThreadPool::instance->tasks_.pop();
          }
          task();
        }
      });
    }
  });
  return ThreadPool::instance;
}

void ThreadPool::joinAll() {
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