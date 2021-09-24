#include "TaskExcutor.h"

std::once_flag TaskExcutor::constructFlag;

void TaskExcutor::polling() {
  using namespace std::chrono_literals;
  while (true) {
    {
      std::unique_lock<std::mutex> ul(this->mtx_);
      this->cv_.wait_for(ul, minPollingInterval * 1ms, [this]() {
        return !this->schedulerPolingList.empty();
      });
      this->minPollingInterval = INT_MAX;
      for (auto schedulerIt = this->schedulerPolingList.begin();
           schedulerIt != this->schedulerPolingList.end(); schedulerIt++) {
        if ((*schedulerIt)->getTaskCount() == 0) {
          auto scherdulerPtr = (*schedulerIt);
          schedulerPolingList.erase(schedulerIt);
          schedulerWaitingList.push_back(scherdulerPtr);
          continue;
        }
        auto [TaskList, interval] = (*schedulerIt)->getReadyTask();
        for (auto task : TaskList) {
          this->pool->submit(task);
        }
        minPollingInterval = std::min(minPollingInterval, interval);
      }
    }
  }
}