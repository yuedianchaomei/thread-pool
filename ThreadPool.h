#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <future>

class ThreadPool
{
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    // 多线程安全
    bool stop;
    std::mutex mutex;
    std::condition_variable cv;

public:
    ThreadPool(size_t);
    ~ThreadPool();
    template<class F, class ...Args>
    auto enqueue(F &&, Args &&...) -> std::future<typename std::invoke_result_t<F, Args...>>;
};

ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for(size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back([this]()
        {
            while(true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(mutex);
                    cv.wait(lock, 
                        [this]()
                        {
                            return stop || !tasks.empty();
                        });
                    if (stop && tasks.empty())
                    {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
            
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }

    cv.notify_all();
    size_t n = workers.size();
    for(size_t i = 0; i < n; ++i)
    {
        workers[i].join();
        std::cout << "workers[" << i << "]已经被销毁。" << std::endl;
    }
}

template<class F, class ...Args>
auto ThreadPool::enqueue(F &&f, Args &&...args) -> std::future<typename std::invoke_result_t<F, Args...>>
{
    using resType = typename std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<resType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<resType> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(mutex);

        if (stop)
        {
            throw std::runtime_error("无法向已停用的线程池中添加任务。");
        }

        tasks.emplace([task]()
        {
            (*task)();
        });
    }

    cv.notify_one();
    return res;
}