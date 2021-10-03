#ifndef __TIMED_TASK_EXECUTOR_H__
#define __TIMED_TASK_EXECUTOR_H__
/**
 * @file TimedTaskExecutor.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 定时任务执行器
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "GlobalTaskManager.h"
#include "TaskScheduler/TimedScheduler/TimedTaskSchedulerProxy.h"

#include <functional>
#include <future>
#include <memory>
#include <thread>

/**
 * @brief 定时任务执行器
 * 
 */
class TimedTaskExecutor {
private:
  /**
   * @brief 单例模式，私有构造函数
   *
   */
  TimedTaskExecutor() : scheduler(new TimedTaskSchedulerProxy()) {
    GlobalTaskManager::addScheduler(this->scheduler->getSchedulerBase());
  };

public:
  /**
   * @brief 获取定时执行器对象实例
   *
   * @return TimedTaskExecutor* 指向实例的指针
   */
  static TimedTaskExecutor *getInstance();

  /**
   * @brief 析构函数，同时从全局TaskManager中移除该调度器
   *
   */
  ~TimedTaskExecutor() {
    GlobalTaskManager::removeScheduler(this->scheduler->getSchedulerBase());
  };

  /**
   * @brief 添加带future的定时任务（不支持重复任务）
   *
   * @tparam F callable
   * @tparam Args 转发参数
   * @param timeout 定时时间
   * @param f callable对象
   * @param args callable对象参数
   * @return std::pair<int, std::future<decltype(f(args...))>> 任务id和furture
   */
  template <typename F, typename... Args>
  static auto addTaskWithFuture(int timeout, F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id =
        instance->scheduler->addTask([taskPtr]() { (*taskPtr)(); }, timeout);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(
        instance->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }

  /**
   * @brief 添加普通定时任务
   *
   * @tparam F callable
   * @tparam Args any
   * @param timeout 定时时间
   * @param repeatTask 是否重复任务
   * @param f callable对象
   * @param args callable对象参数
   * @return int 任务id
   */
  template <typename F, typename... Args>
  static auto addTask(int timeout, bool repeatTask, F &&f, Args &&... args)
      -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = instance->scheduler->addTask(task, timeout, repeatTask);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(
        instance->scheduler->getSchedulerBase());
    return id;
  }

  /**
   * @brief 调整任务属性
   *
   * @param id 任务id
   * @param timeout 新的定时时间
   * @param isRepeatTask 是否重复任务
   * @return int 成功返回id，失败返回-1
   */
  int adjustTask(int id, int timeout, bool isRepeatTask) {
    return scheduler->adjustTask(id, timeout, isRepeatTask);
  }

  /**
   * @brief 调整任务定时时长
   *
   * @param id 任务id
   * @param timeout 新的定时时间
   * @return int 成功返回id，失败返回-1
   */
  int adjustTask(int id, int timeout) {
    return scheduler->adjustTask(id, timeout);
  }

  /**
   * @brief 调整任务是否重复执行
   *
   * @param id 任务id
   * @param isRepeatTask 是否重复任务
   * @return int 成功返回id，失败返回-1
   */
  int adjustTask(int id, bool isRepeatTask) {
    return scheduler->adjustTask(id, isRepeatTask);
  }

  /**
   * @brief 删除任务
   *
   * @param id 任务id
   * @param doCall 是否在删除前执行任务
   * @return int 成功返回id，失败返回-1
   */
  int removeTask(int id, bool doCall) {
    return scheduler->removeTask(id, doCall);
  }

  /**
   * @brief 清空任务列表
   *
   */
  void clearTask() { scheduler->clearTask(); }

private:
  std::unique_ptr<TimedTaskSchedulerProxy> scheduler; //调度器实例

private:
  static std::once_flag constructFlag; //单次构造flag
  static TimedTaskExecutor *instance;  //单例指针

  // ------------  自动析构  ------------ //
private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (TimedTaskExecutor::instance != NULL) {
        delete TimedTaskExecutor::instance;
        TimedTaskExecutor::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif