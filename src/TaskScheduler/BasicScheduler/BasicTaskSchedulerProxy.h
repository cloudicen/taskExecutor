#ifndef __BASIC_TASK_SCHEDULER_PROXY_H__
#define __BASIC_TASK_SCHEDULER_PROXY_H__

/**
 * @file BasicTaskSchedulerProxy.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 基本任务调度器代理类，用于限定传入参数
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "impl/BasicTaskScheduler.h"
#include <memory>

/**
 * @brief 基本任务调度器代理类，用于限定传入参数
 *
 */
class BasicTaskSchedulerProxy {
private:
  std::unique_ptr<BasicTaskScheduler> BasicSchedulerBase;//实际的调度器实例

public:
  BasicTaskSchedulerProxy() : BasicSchedulerBase(new BasicTaskScheduler()){};
  ~BasicTaskSchedulerProxy() = default;

/**
 * @brief 添加任务
 * 
 * @param task void()类型的callable对象
 * @return int 成功返回任务id，失败返回-1
 */
  int addTask(std::function<void()>&& task) {
    return BasicSchedulerBase->addTask(std::forward<std::function<void()>&&>(task));
  }

  /**
   * @brief 清空任务调度器
   *
   */
  void clearTask() { BasicSchedulerBase->clearTask(); }

  /**
   * @brief 获取任务数量
   *
   * @return int 任务数量
   */
  int getTaskCount() { return BasicSchedulerBase->getTaskCount(); }

  /**
   * @brief 获取就绪任务列表
   *
   * @return std::pair<std::vector<std::function<void()>>, int>
   * <任务列表,下个就绪任务时间(ms)>
   */
  std::pair<std::vector<std::function<void()>>, int> getReadyTask() {
    return BasicSchedulerBase->getReadyTask();
  }

  /**
   * @brief 获取实际的调度器实例指针
   *
   * @return TaskSchedulerBase* 调度器实例指针
   */
  TaskSchedulerBase *getSchedulerBase() { return BasicSchedulerBase.get(); }
};

#endif