#ifndef __BASIC_TASK_SCHEDULER_H__
#define __BASIC_TASK_SCHEDULER_H__

/**
 * @file BasicTaskScheduler.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 基本任务调度器
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include "../../TaskSchedulerBase.h"

/**
 * @brief 基本任务调度器
 * 
 */
class BasicTaskScheduler : public TaskSchedulerBase {
public:
  BasicTaskScheduler() = default;
  ~BasicTaskScheduler() = default;
  BasicTaskScheduler(const BasicTaskScheduler &) = delete;

  /**
   * @brief 向调度器内添加任务
   *
   * @param options 额外选项
   * @return int 成功返回分配的任务id，失败返回-1
   */
  int addTask(std::function<void()> &&Task,
              std::initializer_list<void *> options = {}) override;

  int adjustTask(int id, std::initializer_list<void *> options = {}) override {
    return 0;
  };
  int removeTask(int id, bool doCall) override { return 0; };

  /**
   * @brief 清空任务调度器
   *
   */
  void clearTask() override;

  /**
   * @brief 获取任务数量
   *
   * @return int 任务数量
   */
  int getTaskCount() override;

  /**
   * @brief 获取就绪任务列表
   *
   * @return std::pair<std::vector<std::function<void()>>, int>
   * <任务列表,下个就绪任务时间(ms)>
   */
  std::pair<std::vector<std::function<void()>>, int> getReadyTask() override;

private:
  std::vector<std::function<void()>> taskList; //任务列表
  std::shared_mutex mutex;                     //实例全局互斥量
};

#endif