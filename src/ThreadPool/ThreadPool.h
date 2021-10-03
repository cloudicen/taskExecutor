#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/**
 * @brief 线程池
 * 
 */
class ThreadPool {
private:
  /**
   * @brief 单例模式，私有构造函数
   *
   */
  ThreadPool() : stopFlag(false){};

public:
  /**
   * @brief 获取线程池实例
   *
   * @param threadNum 初始化总工作线程数量
   * @return ThreadPool* 实例指针
   */
  static ThreadPool *getInstance(size_t threadNum = 8);

  /**
   * @brief 析构线程池对象，并等待所有任务执行完毕
   *
   */
  ~ThreadPool() { joinAll(); };

  /**
   * @brief join所有工作线程
   *
   */
  static void joinAll();

  /**
   * @brief 提交一个任务到任务队列
   *
   * @param task void()类型的callable对象
   */
  static void submit(std::function<void()> task);

private:
  bool stopFlag;                            //服务停止标志位
  std::vector<std::thread> workers_;        //工作线程列表
  std::queue<std::function<void()>> tasks_; //任务队列

  std::mutex mtx_;             //本对象全局互斥量
  std::condition_variable cv_; //控制任务分配的条件变量

private:
  static std::once_flag constructFlag; //单次构造flag
  static ThreadPool *instance;         //单例指针

  // ------------  自动析构  ------------ //
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
