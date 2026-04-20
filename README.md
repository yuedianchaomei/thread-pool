ThreadPool
===

一个简易线程池

使用方法：
```c++
ThreadPool pool(4);

auto result = pool.enqueue([](int answer)
{
  return answer;
}, 1);

std::cout << result.get() << std::endl;

```

SafeQueue
===

一个简易线程安全队列

使用方法：
```c++
SafeQueue<int> q;

std::thread productor([&q]()
{
    for(int i = 0; i < 5; ++i)
    {
        q.push(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
});

std::thread consumer([&q]()
{
    int val;
    while(true)
    {
        if (q.waitPop(val, std::chrono::milliseconds(1000)))
        {
          std::cout << "消费：" << val << std::endl;
        }
        else
        {
            std::cout << "超时，退出消费。" << std::endl;
            break;
        }
    }
});

productor.join();
consumer.join();
```

PoolWithSafeQueue
===
使用线程安全队列的线程池

使用方法
```c++
PoolWithSafeQueue poolWSQ(4);
std::vector<std::future<int>> results;

for (int i = 0; i < 10; ++i) 
{
    results.emplace_back(poolWSQ.enqueue([](int answer) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return answer;
    }, i));
}

for (auto& result : results) 
{
    std::cout << result.get() << std::endl;
}
```