#ifndef __HEAP_TIMER__
#define __HEAP_TIMER__

#include <map>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <queue>

class HeapTimer;

class TimerNode{
private:
    int id;
    std::chrono::time_point<std::chrono::system_clock> expireTime;
    std::function<void()> callBack;
    friend HeapTimer;
public:
    explicit TimerNode(int _id,std::chrono::time_point<std::chrono::system_clock> _expireTime, std::function<void()> _callBack):id(_id),expireTime(_expireTime),callBack(_callBack){};
    ~TimerNode() = default;

    bool operator>(const TimerNode& other) const{
        return this->expireTime > other.expireTime;
    }
};

class HeapTimer
{
private:
    std::vector<const TimerNode*> timerHeap;
    std::unordered_map<int,std::unique_ptr<TimerNode>> nodeRegestry;
    int currentNodeId = 0;
    std::priority_queue<int> reusedId;
    
    void maintainHeap();
    void pushHeap(const TimerNode* node);
    void popHeap();

public:
    HeapTimer() = default;
    ~HeapTimer() = default;
    HeapTimer(const HeapTimer&) = delete;

    int add(int timeout,std::function<void()> callBack);

    int adjust(int id,int timeout);

    int remove(int id, bool doCall);

    void clear();

    void tick();

    int getNextTickInterval();

    int getTaskCount();
};

#endif

