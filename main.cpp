#include <iostream>

#include "ThreadPool.h"
#include "SafeQueue.h"
#include "PoolWithSafeQueue.h"

int main() 
{
    // // ThreadPool
    // ThreadPool pool(4);
    // std::vector<std::future<int>> results;

    // for (int i = 0; i < 10; ++i) 
    // {
    //     results.emplace_back(pool.enqueue([](int answer) 
    //     {
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //         return answer;
    //     }, i));
    // }

    // for (auto& result : results) 
    // {
    //     std::cout << result.get() << std::endl;
    // }

    // // SafeQueue
    // std::mutex printMutex;
    // auto safePrint = [&printMutex](const std::string &msg)
    // {
    //     std::lock_guard<std::mutex> lock(printMutex);
    //     std::cout << msg << std::endl;
    // };

    // SafeQueue<int> q;

    // std::thread productor([&]()
    // {
    //     for(int i = 0; i < 5; ++i)
    //     {
    //         q.push(i);
    //         safePrint("生产：" + std::to_string(i));
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     }
    // });

    // std::thread consumer([&]()
    // {
    //     int val;
    //     while(true)
    //     {
    //         if (q.waitPop(val, std::chrono::milliseconds(1000)))
    //         {
    //             safePrint("消费：" + std::to_string(val));
    //         }
    //         else
    //         {
    //             safePrint("超时，退出消费。");
    //             break;
    //         }
    //     }
    // });

    // productor.join();
    // consumer.join();

    // PoolWithSafeQueue
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
    
    return 0;
}