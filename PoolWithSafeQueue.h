#pragma once

class PoolWithSafeQueue
{
private:
    std::vector<std::thread> workers;
    SafeQueue<std::function<void()>> tasks;
    bool stop = false;
    std::mutex stopMutex;

public:
    PoolWithSafeQueue(size_t);
    ~PoolWithSafeQueue();

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>>;
};

PoolWithSafeQueue::PoolWithSafeQueue(size_t threads)
{
    for(size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back([this]()
        {
            std::function<void()> task;
            while(tasks.pop(task))
            {
                task();
            }
        });
    }
}

PoolWithSafeQueue::~PoolWithSafeQueue()
{
    {
        std::lock_guard<std::mutex> lock(stopMutex);
        stop = true;
    }

    tasks.setStop(true);

    size_t n = workers.size();
    for(size_t i = 0; i < n; ++i)
    {
        if (workers[i].joinable())
        {
            workers[i].join();
            std::cout << "workers[" << i << "]已被销毁。" << std::endl;
        }
    }
}

template<typename F, typename... Args>
auto PoolWithSafeQueue::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
    using resType = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<resType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<resType> res = task->get_future();

    {
        std::lock_guard<std::mutex> lock(stopMutex);
        if (stop)
        {
            throw std::runtime_error("无法向已停用的线程池中添加任务。");
        }
    }

    tasks.push([task]()
    {
        (*task)();
    });

    return res;
}