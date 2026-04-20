#pragma once

template<typename T>
class SafeQueue
{
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;
    bool stop = false;

public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    void setStop(bool val)
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = val;
        cv.notify_all();
    }

    template<typename U>
    void push(U&& val)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.emplace(std::forward<U>(val));
        cv.notify_one();
    }

    bool pop(T& val)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]()
        {
            return stop || !queue.empty();
        });
        if (stop && queue.empty())
        {
            return false;
        }
        val = std::move(queue.front());
        queue.pop();
        return true;
    }

    bool tryPop(T& val)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty())
        {
            return false;
        }
        val = std::move(queue.front());
        queue.pop();
        return true;
    }

    bool waitPop(T &val, std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mutex);
        bool canPop = cv.wait_for(lock, timeout, [this]()
        {
            return stop || !queue.empty();
        });
        if (!canPop || (stop && queue.empty()))
        {
            return false;
        }
        val = std::move(queue.front());
        queue.pop();
        return true;
    }
};