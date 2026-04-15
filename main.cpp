#include <iostream>
#include "ThreadPool.h"

int main() 
{
    // ThreadPool
    ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; ++i) 
    {
        results.emplace_back(pool.enqueue([](int answer) 
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