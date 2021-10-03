#ifndef __BASIC_TASK_EXECUTOR_H__
#define __BASIC_TASK_EXECUTOR_H__
/**
 * @file BasicTaskExecutor.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 基本任务执行器
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "GlobalTaskManager.h"
#include "TaskScheduler/BasicScheduler/BasicTaskSchedulerProxy.h"

#include <functional>
#include <future>
#include <memory>
#include <thread>

/**
 * @brief 基本任务执行器
 * 
 */
class BasicTaskExecutor {
private:
  /**
   * @brief 单例模式，私有构造函数
   *
   */
  BasicTaskExecutor() : scheduler(new BasicTaskSchedulerProxy()) {
    GlobalTaskManager::addScheduler(this->scheduler->getSchedulerBase());
  };

public:
  /**
   * @brief 获取基本执行器对象实例
   *
   * @return BasicTaskExecutor* 指向实例的指针
   */
  static BasicTaskExecutor *getInstance();

  /**
   * @brief 析构函数，同时从全局TaskManager中移除该调度器
   *
   */
  ~BasicTaskExecutor() {
    GlobalTaskManager::removeScheduler(this->scheduler->getSchedulerBase());
  };

  /**
   * @brief 添加带future的普通任务
   *
   * @tparam F callable
   * @tparam Args 转发参数
   * @param f callable对象
   * @param args callable对象参数
   * @return std::pair<int, std::future<decltype(f(args...))>> 任务id和furture
   */
  template <typename F, typename... Args>
  static auto addTaskWithFuture(F &&f, Args &&... args)
      -> std::pair<int, std::future<decltype(f(args...))>> {
    auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    int id = instance->scheduler->addTask([taskPtr]() { (*taskPtr)(); });
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(
        instance->scheduler->getSchedulerBase());
    return {id, taskPtr->get_future()};
  }
  /**
   * @brief 添加普通任务
   *
   * @tparam F callable
   * @tparam Args any
   * @param f callable对象
   * @param args callable对象参数
   * @return int 任务id
   */
  template <typename F, typename... Args>
  static auto addTask(F &&f, Args &&... args) -> int {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int id = instance->scheduler->addTask(task);
    //通知任务调度管理器，有新任务加入
    GlobalTaskManager::schedulerOnNewTask(
        instance->scheduler->getSchedulerBase());
    return id;
  }
  /**
   * @brief 清空任务列表
   *
   */
  void clearTask() { scheduler->clearTask(); }

private:
  std::unique_ptr<BasicTaskSchedulerProxy> scheduler; //调度器实例

private:
  static std::once_flag constructFlag; //单次构造flag
  static BasicTaskExecutor *instance;  //单例指针

  // ------------  自动析构  ------------ //
private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (BasicTaskExecutor::instance != NULL) {
        delete BasicTaskExecutor::instance;
        BasicTaskExecutor::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif