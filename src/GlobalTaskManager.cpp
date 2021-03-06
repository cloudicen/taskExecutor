#include "GlobalTaskManager.h"

std::once_flag GlobalTaskManager::constructFlag;
GlobalTaskManager *GlobalTaskManager::instance;

GlobalTaskManager *GlobalTaskManager::getInstance() {
  if (instance != nullptr) {
    return instance;
  }
  std::call_once(GlobalTaskManager::constructFlag, []() {
    instance = new GlobalTaskManager();
    instance->managerThread = new std::thread([]() { instance->polling(); });
  });
  return instance;
}

void GlobalTaskManager::stopService() {
  assert(instance != nullptr);
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    instance->stopFlag = true;
  }
  instance->cv_.notify_all();
  instance->managerThread->join();
}

void GlobalTaskManager::polling() {
  using namespace std::chrono_literals;
  while (true) {
    {
      std::unique_lock<std::mutex> ul(this->mtx_);
      //当等待时间不为0且轮询队列不为空时才跳过等待，或者是停止标志位被设置
      this->cv_.wait_for(ul, pollingInterval * 1ms, [this]() {
        return (pollingInterval <= 0 && !this->schedulerPolingList.empty()) ||
               stopFlag;
      });

      //停止标志位被设置，等待所有任务均调度完成
      if (stopFlag && this->schedulerPolingList.empty()) {
        return;
      }

      this->pollingInterval = INT32_MAX;

      for (auto schedulerIt = this->schedulerPolingList.begin();
           schedulerIt != this->schedulerPolingList.end();) {
        //若调度器内无事件，则移入等待队列。调度器添加事件时需要唤醒
        if ((*schedulerIt)->getTaskCount() == 0) {
          auto scherdulerToMove = schedulerPolingList.extract(schedulerIt++);
          schedulerWaitingList.insert(std::move(scherdulerToMove));
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
        schedulerIt++;
      }
    }
  }
}

void GlobalTaskManager::addScheduler(TaskSchedulerBase *schedulerPtr) {
  assert(schedulerPtr != nullptr);
  auto instance = GlobalTaskManager::getInstance();
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    auto node = instance->schedulerWaitingList.extract(schedulerPtr);
    if (!node.empty()) {
      return;
    }
    instance->schedulerPolingList.insert(std::move(schedulerPtr));
    // 新任务调度器加入，需要立即查询任务就绪列表，将等待超时时长置0
    instance->pollingInterval = 0;
  }
  instance->cv_.notify_one();
}

void GlobalTaskManager::removeScheduler(TaskSchedulerBase *schedulerPtr) {
  assert(schedulerPtr != nullptr);
  auto instance = GlobalTaskManager::getInstance();
  std::unique_lock<std::mutex> ul(instance->mtx_);
  instance->schedulerWaitingList.erase(schedulerPtr);
  instance->schedulerPolingList.erase(schedulerPtr);
}

void GlobalTaskManager::schedulerOnNewTask(TaskSchedulerBase *schedulerPtr) {
  assert(schedulerPtr != nullptr);
  auto instance = GlobalTaskManager::getInstance();
  {
    std::unique_lock<std::mutex> ul(instance->mtx_);
    auto node = instance->schedulerWaitingList.extract(schedulerPtr);
    if (!node.empty()) {
      instance->schedulerPolingList.insert(std::move(node));
    } else {
      if (instance->schedulerPolingList.count(schedulerPtr) == 0) {
        return;
      }
    }
    // 新任务加入，需要立即查询任务就绪列表，将等待超时时长置0
    instance->pollingInterval = 0;
  }
  instance->cv_.notify_one();
}