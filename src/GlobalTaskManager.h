#ifndef __GLOBAL_TASK_MANAGER_H__
#define __GLOBAL_TASK_MANAGER_H__
/**
 * @file GlobalTaskManager.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 全局任务管理器
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "TaskScheduler/TaskSchedulerBase.h"
#include "ThreadPool/ThreadPool.h"

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <set>
#include <thread>

/**
 * @brief 全局任务管理器
 * 
 */
class GlobalTaskManager {
private:
  /**
   * @brief 单例模式，私有构造函数
   *
   */
  GlobalTaskManager() = default;

  /**
   * @brief 轮询任务调度器
   *
   */
  void polling();

public:
  /**
   * @brief 获取TaskManager实例
   *
   * @return GlobalTaskManager*
   */
  static GlobalTaskManager *getInstance();

  /**
   * @brief 析构对象，同时停止服务，等待所有任务执行完毕
   *
   */
  ~GlobalTaskManager() { stopService(); };

  /**
   * @brief 停止服务，等待所有任务执行完毕
   *
   */
  static void stopService();

  /**
   * @brief 添加任务调度器
   *
   */
  static void addScheduler(TaskSchedulerBase *);

  /**
   * @brief 移除任务调度器
   *
   */
  static void removeScheduler(TaskSchedulerBase *);

  /**
   * @brief 调度器通知有新任务加入
   *
   */
  static void schedulerOnNewTask(TaskSchedulerBase *);

private:
  bool stopFlag = false;                              //服务停止标志位
  int pollingInterval = 0;                            //轮询时间
  std::set<TaskSchedulerBase *> schedulerPolingList;  //轮询列表
  std::set<TaskSchedulerBase *> schedulerWaitingList; //等待列表

  std::thread *managerThread;  //任务管理器独立线程
  std::mutex mtx_;             //本对象全局互斥量
  std::condition_variable cv_; //控制轮询的条件变量

private:
  static std::once_flag constructFlag; //单次构造flag
  static GlobalTaskManager *instance;  //单例指针

  // ------------  自动析构  ------------ //
private:
  class Garbo //设置为私有防止外界访问
  {
  public:
    ~Garbo() //实际去析构new的单例对象
    {
      if (GlobalTaskManager::instance != NULL) {
        delete GlobalTaskManager::instance;
        GlobalTaskManager::instance = nullptr;
      }
    }
  };
  static Garbo garbo;
};

#endif