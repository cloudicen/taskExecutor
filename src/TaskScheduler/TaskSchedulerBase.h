#ifndef __TASK_SCHEDULER_BASE_H__
#define __TASK_SCHEDULER_BASE_H__
/**
 * @file TaskSchedulerBase.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 任务调度器基类
 * @version 0.1
 * @date 2021-10-03
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <functional>
#include <future>
#include <memory>
#include <queue>

/**
 * @brief 任务调度器基类
 *
 */
class TaskSchedulerBase {
protected:
  int currentTaskId = 0;             //当前已分配的任务id（最大id)
  std::priority_queue<int> reusedId; //回收重用任务id列表

  /**
   * @brief 获得一个可用的任务id
   *
   * @return int 任务id
   */
  int getNewTaskId() {
    int id = 0;
    if (this->reusedId.empty()) {
      id = currentTaskId++;
    } else {
      id = this->reusedId.top();
      this->reusedId.pop();
    }
    return id;
  }

  /**
   * @brief  重置任务id
   *
   */
  void resetTaskId() {
    currentTaskId = 0;
    reusedId = std::priority_queue<int>();
  }

public:
  TaskSchedulerBase() = default;
  virtual ~TaskSchedulerBase() = default;

  /**
   * @brief 向调度器内添加任务
   *
   * @param options 额外选项
   * @return int 成功返回分配的任务id，失败返回-1
   */
  virtual int addTask(std::function<void()> &&,
                      std::initializer_list<void *> options) = 0;

  /**
   * @brief   移除一个任务
   *
   * @param taskId 任务id
   * @param doCall 移除之前是否执行任务
   * @return int 成功返回任务id，失败返回-1
   */
  virtual int removeTask(int taskId, bool doCall) = 0;

  /**
   * @brief 调整任务参数
   *
   * @param taskId 任务id
   * @param options 额外参数选项
   * @return int 成功返回任务id，失败返回-1
   */
  virtual int adjustTask(int taskId, std::initializer_list<void *> options) = 0;

  /**
   * @brief 清空任务调度器
   *
   */
  virtual void clearTask() = 0;

  /**
   * @brief 获取任务数量
   *
   * @return int 任务数量
   */
  virtual int getTaskCount() = 0;

  /**
   * @brief 获取就绪任务列表
   *
   * @return std::pair<std::vector<std::function<void()>>, int>
   * <任务列表,下个就绪任务时间(ms)>
   */
  virtual std::pair<std::vector<std::function<void()>>, int> getReadyTask() = 0;
};

#endif