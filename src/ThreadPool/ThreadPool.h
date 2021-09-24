#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
private:
  ThreadPool() : stop_(false){};

public:
  ~ThreadPool() { joinAll(); };
  static ThreadPool *getInstance(size_t threadNum = 8);
  static void joinAll();
  void submit(std::function<void()> task);

private:
  bool stop_;
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex mtx_;
  std::condition_variable cv_;
  static std::once_flag threadPoolConstruct;
};

#endif
