//#include <iostream>           // std::cout
//#include <thread>             // std::thread
//#include <chrono>
//#include <future>
//#include <typeinfo>
//#include "SafeQueue.h"
//
//
//using namespace std;
//
//
////普通函数
//int madd(int x, int y)
//{
//    cout << x + y << endl;
//    return x + y;
//}
//
//void hello() { cout << "hello world" << endl; }
//typedef void (*fp)(void);
//
//void task_lambda()
//{
//    //包装可调用目标时lambda
//    packaged_task<int(int, int)> task([](int a, int b) { return a + b; });
//
//    //仿函数形式，启动任务
//    task(2, 10);
//
//    //获取共享状态中的值,直到ready才能返回结果或者异常
//    future<int> result = task.get_future();
//    cout << "task_lambda :" << result.get() << "\n";
//}
//
//
//void task_thread()
//{
//    //包装普通函数
//    std::packaged_task<int(int, int)> task(madd);
//    future<int> result = task.get_future();
//    //启动任务，非异步
//    task(4, 8);
//    cout << "task_thread :" << result.get() << "\n";
//
//    //重置共享状态
//    task.reset();
//    result = task.get_future();
//
//    //通过线程启动任务，异步启动
//    thread td(move(task), 2, 10);
//    td.join();
//    //获取执行结果
//    cout << "task_thread :" << result.get() << "\n";
//}
//
//
//// 编写一个函数，接受任意的 函数 和 对应参数，并且能够正常执行
//template <typename F, typename... Args>
//auto work(F&& fun, Args&&... args) -> std::future<decltype(fun(args...))>
//{
//    // 函数绑定
//    auto f = bind(forward<F>(fun), forward<Args>(args)...);
//    packaged_task<decltype(f())()> task(f);
//    task();
//    return move(task.get_future());
//}
//
//void test()
//{
//    /* task_lambda();
//    task_thread();*/
//
//    auto fu = work(madd, 2, 3);
//
//    cout << "test " << fu.get() << endl;
//}
//
//int main(int argc, char* argv[])
//{
//    //test();
//    SafeQueue<std::function<void()>> queue;
//    auto f = bind(madd, 2, 3);
//    auto task_ptr = std::make_unique<packaged_task<decltype(f())()>> (f);
//    std::function<void()> wrapper_func = [&task_ptr]() {
//        (*task_ptr)();
//    };
//    queue.Push(wrapper_func);
//    auto t = queue.Pop();
//    t();
//    return 0;
//}


#include <iostream>
#include "mThreadPool.h"

int madd(int x, int y)
{
    cout << x + y << endl;
    return x + y;
}

void prt()
{
	std::cout << "hello" << std::endl;
}

void test1()
{
	mThreadPool pl(10);
	for (int i = 0; i < 30; ++i)
		pl.addtask(madd, 2, 5);
	pl.start();
	for (int i = 0; i < 30; ++i)
		pl.addtask(prt);
	pl.shutdown();

	std::cout << pl.exc_num << std::endl;
}

int main()
{
	//mThreadPool pl(2);
	////for (int i = 0; i < 30; ++i)
	//pl.addtask(madd, 5, 6);
	//pl.addtask(prt);
	//pl.start();
	//pl.shutdown();


	//std::cout << pl.exc_num << std::endl;
	test1();
	return 0;
}

