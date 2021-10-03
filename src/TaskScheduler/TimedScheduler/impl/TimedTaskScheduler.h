#ifndef __TIMED_TASK_SCHEDULER_H__
#define __TIMED_TASK_SCHEDULER_H__
/**
 * @file TimedTaskScheduler.h
 * @author cloudicen cloudicen@foxmail.com)
 * @brief 定时任务调度器实现
 * @version 0.1
 * @date 2021-10-03
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <assert.h>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "../../TaskSchedulerBase.h"

class TimedTaskScheduler;

/**
 * @brief 定时任务节点
 *
 */
class TimedTaskNode {
public:
  explicit TimedTaskNode(
      int _id, std::chrono::time_point<std::chrono::system_clock> _expireTime,
      std::function<void()> &&_callBack, int _interval, bool _repeat)
      : id(_id), expireTime(_expireTime), callBack(_callBack),
        interval(_interval), repeatTask(_repeat){};
  ~TimedTaskNode() = default;

  bool operator>(const TimedTaskNode &other) const {
    return this->expireTime > other.expireTime;
  }

private:
  int id;                                                        //任务id
  std::chrono::time_point<std::chrono::system_clock> expireTime; //过期时间
  int interval;                   //执行间隔(ms)
  std::function<void()> callBack; //任务回调函数
  bool repeatTask;                //是否为重复任务

  //友元类
  friend TimedTaskScheduler;
};

/**
 * @brief 定时任务调度器
 *
 */
class TimedTaskScheduler : public TaskSchedulerBase {
private:
  /**
   * @brief 维护任务节点小顶堆
   *
   */
  void maintainHeap();

  /**
   * @brief 任务节点入堆
   *
   * @param node 节点指针
   */
  void pushHeap(TimedTaskNode *node);

  /**
   * @brief 任务节点出堆
   *
   */
  void popHeap();

public:
  TimedTaskScheduler() = default;
  ~TimedTaskScheduler() = default;
  TimedTaskScheduler(const TimedTaskScheduler &) = delete;

  /**
   * @brief 向调度器内添加任务
   *
   * @param options 额外选项
   * @return int 成功返回分配的任务id，失败返回-1
   */
  int addTask(std::function<void()> &&Task,
              std::initializer_list<void *> options) override;

  /**
   * @brief 调整任务参数
   *
   * @param id 任务id
   * @param timeout 新等待时间
   * @param isRepeatTask 是否为重复任务
   * @return int 成功返回任务id，失败返回-1
   */
  int adjustTask(int id, std::initializer_list<void *> options) override;

  /**
   * @brief   移除一个任务
   *
   * @param taskId 任务id
   * @param doCall 移除之前是否执行任务
   * @return int 成功返回任务id，失败返回-1
   */
  int removeTask(int id, bool doCall) override;

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
  std::vector<TimedTaskNode *> timerHeap; //时间排序的小顶堆
  std::unordered_map<int, std::unique_ptr<TimedTaskNode>>
      nodeRegestry;        //任务节点列表
  std::shared_mutex mutex; //实例全局互斥量
};

#endif
