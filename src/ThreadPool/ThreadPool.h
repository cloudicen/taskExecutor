#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <cassert>
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
  static void submit(std::function<void()> task);

private:
  bool stop_;
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex mtx_;
  std::condition_variable cv_;
  static std::once_flag threadPoolConstruct;

private:
  static ThreadPool *instance;

private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (ThreadPool::instance != NULL) {
        delete ThreadPool::instance;
        ThreadPool::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif
