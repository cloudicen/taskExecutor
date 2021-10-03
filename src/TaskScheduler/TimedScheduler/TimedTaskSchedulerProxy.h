#ifndef __TIMED_TASK_SCHEDULER_PROXY_H__
#define __TIMED_TASK_SCHEDULER_PROXY_H__
/**
 * @file TimedTaskSchedulerProxy.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 定时任务调度器代理类，用于限定传入参数
 * @version 0.1
 * @date 2021-10-03
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "impl/TimedTaskScheduler.h"
#include <memory>

/**
 * @brief 定时任务调度器代理类，用于限定传入参数
 *
 */
class TimedTaskSchedulerProxy {
private:
  std::unique_ptr<TimedTaskScheduler> timedSchedulerBase; //实际的调度器实例

public:
  TimedTaskSchedulerProxy() : timedSchedulerBase(new TimedTaskScheduler()){};
  ~TimedTaskSchedulerProxy() = default;

  /**
   * @brief 添加任务
   *
   * @param task void()类型的callable任务
   * @param timeout 等待时间
   * @param isRepeatTask 是否为重复任务
   * @return int 成功返回任务id，失败返回-1
   */
  int addTask(std::function<void()> &&task, int timeout,
              bool isRepeatTask = false) {
    return timedSchedulerBase->addTask(
        std::forward<std::function<void()> &&>(task),
        {&timeout, &isRepeatTask});
  }

  /**
   * @brief 调整任务参数
   *
   * @param id 任务id
   * @param timeout 新等待时间
   * @param isRepeatTask 是否为重复任务
   * @return int 成功返回任务id，失败返回-1
   */
  int adjustTask(int id, int timeout, bool isRepeatTask) {
    return timedSchedulerBase->adjustTask(id, {&timeout, &isRepeatTask});
  }

  /**
   * @brief 调整任务等待时间
   *
   * @param id 任务id
   * @param timeout 新等待时间
   * @return int 成功返回任务id，失败返回-1
   */
  int adjustTask(int id, int timeout) {
    return timedSchedulerBase->adjustTask(id, {&timeout, nullptr});
  }

  /**
   * @brief 调整任务是否重复
   *
   * @param id 任务id
   * @param isRepeatTask 是否为重复任务
   * @return int 成功返回任务id，失败返回-1
   */
  int adjustTask(int id, bool isRepeatTask) {
    return timedSchedulerBase->adjustTask(id, {nullptr, &isRepeatTask});
  }

  /**
   * @brief 移除一个任务
   *
   * @param id 任务id
   * @param doCall 移除之前是否执行任务
   * @return int 成功返回任务id，失败返回-1
   */
  int removeTask(int id, bool doCall) {
    return timedSchedulerBase->removeTask(id, doCall);
  }

  /**
   * @brief 清空任务调度器
   *
   */
  void clearTask() { timedSchedulerBase->clearTask(); }

  /**
   * @brief 获取任务数量
   *
   * @return int 任务数量
   */
  int getTaskCount() { return timedSchedulerBase->getTaskCount(); }

  /**
   * @brief 获取就绪任务列表
   *
   * @return std::pair<std::vector<std::function<void()>>, int>
   * <任务列表,下个就绪任务时间(ms)>
   */
  std::pair<std::vector<std::function<void()>>, int> getReadyTask() {
    return timedSchedulerBase->getReadyTask();
  }

  /**
   * @brief 获取实际的调度器实例指针
   *
   * @return TaskSchedulerBase* 调度器实例指针
   */
  TaskSchedulerBase *getSchedulerBase() { return timedSchedulerBase.get(); }
};

#endif