#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <queue>
#include <future>

class ThreadPool {
public:
    explicit ThreadPool(size_t threadNum = 8) : stop_(false) {
        for(size_t i = 0; i < threadNum; ++i) {
            workers_.emplace_back([this]() {
                for(;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ul(mtx_);
                        cv_.wait(ul, [this]() { return stop_ || !tasks_.empty(); });
                        if(stop_ && tasks_.empty()) { return; }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> ul(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for(auto& worker : workers_) {
            worker.join();
        }
    }

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        {
            std::unique_lock<std::mutex> ul(mtx_);
            if(stop_) { throw std::runtime_error("submit on stopped ThreadPool"); }
            tasks_.emplace([taskPtr]() { (*taskPtr)(); });
        }
        cv_.notify_one();
        return taskPtr->get_future();
    }

private:
    bool stop_;
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
};

#endif

