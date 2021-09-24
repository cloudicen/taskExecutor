#include "ThreadPool.h"

std::once_flag ThreadPool::threadPoolConstruct;

ThreadPool::ThreadPool(size_t threadNum) : stop_(false) {
  std::call_once(this->threadPoolConstruct, [&]() {
    for (size_t i = 0; i < threadNum; ++i) {
      workers_.emplace_back([this]() {
        for (;;) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> ul(mtx_);
            cv_.wait(ul, [this]() { return stop_ || !tasks_.empty(); });
            if (stop_ && tasks_.empty()) {
              return;
            }
            task = std::move(tasks_.front());
            tasks_.pop();
          }
          task();
        }
      });
    }
  });
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> ul(mtx_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto &worker : workers_) {
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