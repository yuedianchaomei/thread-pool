ThreadPool
==========

一个简易线程池

使用方法:
```c++
ThreadPool pool(4);

auto result = pool.enqueue([](int answer){
  return answer;
}, 1);

std::cout << result.get() << std::endl;

```
