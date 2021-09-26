#include "GlobalTaskManager.h"

std::once_flag GlobalTaskManager::constructFlag;

GlobalTaskManager *GlobalTaskManager::getInstance() {
  if (instance != nullptr) {
    return instance;
  }
  std::call_once(GlobalTaskManager::constructFlag, []() {
    instance = new GlobalTaskManager();
    instance->schedulerThread = new std::thread([]() { instance->polling(); });
  });
  return instance;
}

void GlobalTaskManager::stopExcutor() {
    assert(instance != nullptr);
    {
      std::unique_lock<std::mutex> ul(instance->mtx_);
      instance->stopFlag = true;
    }
    instance->cv_.notify_all();
    instance->schedulerThread->join();
  }

void GlobalTaskManager::polling() {
  using namespace std::chrono_literals;
  while (true) {
    {
      std::unique_lock<std::mutex> ul(this->mtx_);
      //当等待时间不为0且轮询队列不为空时才跳过等待，或者是停止标志位被设置
      this->cv_.wait_for(ul, pollingInterval * 1ms, [this]() {
        return (pollingInterval <= 0 &&
                !this->schedulerPolingList.empty()) ||
               stopFlag;
      });

      //停止标志位被设置，等待所有任务均调度完成
      if (stopFlag && this->schedulerPolingList.empty()) {
        return;
      }

      this->pollingInterval = INT32_MAX;

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
          ThreadPool::submit(task);
        }
        //更新轮询等待时间，为所有调度器返回的最小值。
        pollingInterval = std::min(pollingInterval, interval);
      }
    }
  }
}

void GlobalTaskManager::addScheduler(TaskSchedulerBase *schedulerPtr) {
  assert(schedulerPtr != nullptr);
  auto instance = GlobalTaskManager::getInstance();
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    instance->schedulerWaitingList.remove(schedulerPtr);
    for (auto scheduler : instance->schedulerPolingList) {
      if (scheduler == schedulerPtr) {
        return;
      }
    }
    instance->schedulerPolingList.push_back(schedulerPtr);
    // 新任务调度器加入，需要立即查询任务就绪列表，将等待超时时长置0
    instance->pollingInterval = 0;
  }
  instance->cv_.notify_one();
}

void GlobalTaskManager::removeScheduler(TaskSchedulerBase *schedulerPtr) {
  assert(schedulerPtr != nullptr);
  auto instance =  GlobalTaskManager::getInstance();
  std::unique_lock<std::mutex> ul(instance->mtx_);
  instance->schedulerWaitingList.remove(schedulerPtr);
  instance->schedulerPolingList.remove(schedulerPtr);
}

void GlobalTaskManager::schedulerOnNewTask(TaskSchedulerBase *schedulerPtr) {
  assert(schedulerPtr != nullptr);
  auto instance =  GlobalTaskManager::getInstance();
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    instance->schedulerWaitingList.remove(schedulerPtr);
    bool alreadyHaveFlag = false;
    for (auto scheduler : instance->schedulerPolingList) {
      if (scheduler == schedulerPtr) {
        alreadyHaveFlag = true;
        break;
      }
    }
    if (!alreadyHaveFlag) {
      instance->schedulerPolingList.push_back(schedulerPtr);
    }
    // 新任务加入，需要立即查询任务就绪列表，将等待超时时长置0
    instance->pollingInterval = 0;
  }
  instance->cv_.notify_one();
}