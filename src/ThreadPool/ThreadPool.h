#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
public:
  explicit ThreadPool(size_t threadNum = 8);

  ~ThreadPool();

  void submit(std::function<void()> task);

  template <typename F, typename... Args>
  auto submit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    {
      std::unique_lock<std::mutex> ul(mtx_);
      if (stop_) {
        throw std::runtime_error("submit on stopped ThreadPool");
      }
      tasks_.emplace([taskPtr]() { (*taskPtr)(); });
    }
    cv_.notify_one();
    return taskPtr->get_future();
  }

private:
  bool stop_;
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex mtx_;
  std::condition_variable cv_;
  static std::once_flag threadPoolConstruct;
};

#endif
