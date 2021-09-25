#include "TaskExcutor.h"
#include "iostream"

std::once_flag TaskExcutor::constructFlag;

void TaskExcutor::polling() {
  using namespace std::chrono_literals;
  while (true) {
    {
      std::unique_lock<std::mutex> ul(this->mtx_);
      //当等待时间不为0且轮询队列不为空时才跳过等待，或者是停止标志位被设置
      this->cv_.wait_for(ul, minPollingInterval * 1ms, [this]() {
        return (minPollingInterval <= 0 &&
                !this->schedulerPolingList.empty()) ||
               stop;
      });
      //停止标志位被设置，等待所有任务均调度完成
      if (stop && this->schedulerPolingList.empty()) {
        return;
      }

      this->minPollingInterval = INT32_MAX;

      for (auto schedulerIt = this->schedulerPolingList.begin();
           schedulerIt != this->schedulerPolingList.end(); schedulerIt++) {
        //若调度器内无事件，则移入等待队列。调度器添加事件时需要唤醒
        if ((*schedulerIt)->getTaskCount() == 0) {
          auto scherdulerPtr = (*schedulerIt);
          schedulerIt = schedulerPolingList.erase(schedulerIt);
          schedulerWaitingList.push_back(scherdulerPtr);
          continue;
        }
        //获取就绪任务列表
        auto [TaskList, interval] = (*schedulerIt)->getReadyTask();
        //提交任务到线程池
        for (auto task : TaskList) {
          this->pool->submit(task);
        }
        //更新轮询等待时间，为所有调度器返回的最小值。
        minPollingInterval = std::min(minPollingInterval, interval);
      }
    }
  }
}

void TaskExcutor::addScheduler(TaskSchedulerImplBase *schedulerPtr) {
  {
    std::unique_lock<std::mutex> ul(this->mtx_);
    this->schedulerWaitingList.remove(schedulerPtr);
    for (auto scheduler : this->schedulerPolingList) {
      if (scheduler == schedulerPtr) {
        return;
      }
    }
    this->schedulerPolingList.push_back(schedulerPtr);
    // 新任务调度器加入，需要立即查询任务就绪列表，将等待超时时长置0
    minPollingInterval = 0;
  }
  this->cv_.notify_one();
}

void TaskExcutor::removeScheduler(TaskSchedulerImplBase *schedulerPtr) {
  std::unique_lock<std::mutex> ul(this->mtx_);
  this->schedulerWaitingList.remove(schedulerPtr);
  this->schedulerPolingList.remove(schedulerPtr);
}

void TaskExcutor::schedulerOnNewTask(TaskSchedulerImplBase *schedulerPtr) {
  {
    std::unique_lock<std::mutex> ul(this->mtx_);
    this->schedulerWaitingList.remove(schedulerPtr);
    bool alreadyHaveFlag = false;
    for (auto scheduler : this->schedulerPolingList) {
      if (scheduler == schedulerPtr) {
        alreadyHaveFlag = true;
        break;
      }
    }
    if (!alreadyHaveFlag) {
      this->schedulerPolingList.push_back(schedulerPtr);
    }
    // 新任务加入，需要立即查询任务就绪列表，将等待超时时长置0
    minPollingInterval = 0;
  }
  this->cv_.notify_one();
}