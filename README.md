# 概述

## 项目简介

这是一个<font color=#F63B10>c++线程池项目</font>，实现子线程的自动创建、销毁以及任务的调度。

## 文件描述

`errors.h` 	 : 自定义异常 

`SafeQueue.h` ：包装 queue 实现一个多线程安全的队列。

`mThreadPool.h`	 : 定义线程池类

`mThreadPool.cpp` : 实现 mThreadPool 类的函数

`main.cpp`			: 测试文件，引入自定义的线程池文件，并进行应用测试

## 项目运行环境

vistual studio 2022

## 项目下载

```git

```

## 项目实现解析

### 结构

1. 线程池类 `class mThreadPool`    	： 内部私有式定义和实现类工作线程类、管理线程类。面向用户，提供添加任务、启动线程池、关闭线程池的接口。
2. `任务队列类 class SafeQueue`          ： 提供了队列任务添加、弹出、队列判空、队列元素个数等方法，内部使用了锁，是线程安全的。
3. 工作线程类 `class mThreadWorker` ： 负责任务的执行
4. 管理线程类 `class ManagerThread` ： 负责管理子线程的调度和关闭

### 使用流程

1. 创建一个线程池类，指定线程池中线程的数目（不指定则默认使用默认值 `MIN_POOL_SIZE` ）。
2. 向线程池的任务队列中添加任务。
3. 开启线程池，正式工作，工作线程取出并执行任务队列中的任务。
4. 关闭线程池，管理线等待工作线程将任务队列中的任务全部执行完后，关闭子线程。之后，管理线程关闭。
5. 线程池成功关闭。

### 管理线程的调度

每隔固定时间(`DEFAULT_TIME` )工作一次。

1. <font color=68B3F2>负责异常处理。</font>比如，任务队列不为空，但存在空闲的工作线程。也就是说，空闲工作线程此时应该去取任务执行，而不是空闲等待。如果管理线程发现这种情况，则会唤醒空闲等待的工作线程来执行任务。
2. <font color=68B3F2>工作线程的安全退出。</font> 线程池调用关闭函数，管理线程在确定任务已经被全部执行完毕后，才会关闭子线程。

# Index

## [class SafeQueue](#SafeQueue)

### 	[bool Empty()](#bool Empty())

### 	[int Size()](#int Size())

### [	void Push()](void Push)

## [namespace ErrorType](#namespace ErrorType)

## [class MTheadPoolError](#MTheadPoolError ) 

### 	[what](#char const* MTheadPoolError\<T\>::what() const)

## [class MThreadPool](#mThreadPool)

### 	[void start()](#void start())

### 	[void shutdown()](#void shutdown())

### 	[void addtask(std::function<void()> )](#void shutdown())

## [class mThreadWoker](#mThreadWorker)

## [class ManagerThread](#ManagerThread)

# **SafeQueue**

任务队列，负责存取任务。

```cpp
class SafeQueue
{
private:
    std::queue<T> m_queue; // 存放数据
    std::mutex m_mutex;	   // 互斥量
    
public:
	SafeQueue()  {}	// 空无参构造函数
	~SafeQueue() {} // 空析构函数

	// 判断队列是否为空
	bool Empty();

	// 返回队列中元素的个数
	int Size();

	// 向队列中添加中构造元素
	void Push (T t);
    
private:
	// 弹出队列中的一个元素
	T&& pop();
};
```

#### bool Empty()

判断队列是否为空

#### int Size()

返回队列中元素的个数

#### void Push()

向队列中添加中构造元素

### **namespace ErrorType**

```CPP
namespace ErrorType
{
    enum class SafeQueue // 来自 SafeQueue 的异常
    {
        empty			// 异常类型为empty
    };
}
```

# namespace ErrorType

> 自定义命名空间，存放线程池能抛出的异常对象。
>
> ```cpp
> enum class SafeQueue
> {
>     empty  // SafeQueue 为空异常
> };
> ```

# **MTheadPoolError**

自定义异常类

```cpp
template<typename T>
class MTheadPoolError : public std::exception {
public:
    MTheadPoolError(T _err_type) : err_type(_err_type) {};

    char const* what() const;  // 获取具体的错误信息


private:
    T err_type;     // 异常类型  ErroType中定义的枚举类
    std::stringstream ss;
};

```

## char const* MTheadPoolError\<T\>::what() const

返回自定义异常类的异常信息

# mThreadPool

> 线程池类
>
> ```c++
> class mThreadPool
> {
> private:
>     class mThreadWorker;		// 工作线程类
> 
> public:
>     
>     std::atomic<bool> m_stop;   // 是否关闭提交
> 
>     bool m_shutdown;            // 是否关闭线程池
> 
>     int free_thr_num;           // 工作线程的个数
>     
>     int thr_num;                // 线程池线程数目
>     
> public:
>     // 创建线程池
>     mThreadPool();
>     mThreadPool(int _thr_num);
> 
>     // 退出线程池
>     ~mThreadPool();
> 
>     // 启动线程池
>     void start();
> 
>     // 停止任务提交
>     void stop();
>    
>     // 重启任务提交
>  void restart();
>    
>     // 关闭线程池
>     void shutdown();
>    
>  // 添加任务
>     void addtask(std::function<void()> task) {}
>    };
> ```

## void start()

线程池启动函数。

> 当线程池的准备工作完成后，调用start函数，线程池即刻开始工作，去除任务队列中函数，并执行。

## void shutdown()

线程池关闭函数。

> 当不需要使用线程池的时候，调用shutdown函数，此时线程池将不再支持任务的添加。等任务队列中任务全部结束后，线程池释放所有开启的子线程。

## void addtask(std::function<void()> )

向线程池中添加任务，等待执行。

> std::function<void()>是任务的类型--仿函数，可以向线程中添加这种类型得到任务函数。

# mThreadWorker

> 工作线程，用以执行任务队列中的任务函数。
>
> ```c++
> class mThreadWorker {//内置线程工作类
> 
>     private:
>         int m_id; //工作id
> 
>         mThreadPool* m_pool;//所属线程池
> 
>     public:
>         //构造函数
>         mThreadWorker(mThreadPool* pool, const int id); // 所属线程池的对象的指针、工作线程的编号
> };
> ```

# **ManagerThread**

> 管理线程，复制线程的调度、析构。
>
> ```cpp
> // 内置管理类
> class ManagerThread
>     {
>     private:
>         mThreadPool* pool_ptr;		// 存放外部线程池对象的 this 指针
>     public:
>         ManagerThread(mThreadPool*);// 传入 ManagerThread 所属的线程池的指针
>     };
>    
> ```

# **参考链接**

[C++ 线程池_c++线程池_破戒僧的博客-CSDN博客](https://blog.csdn.net/qq_34771252/article/details/90319537?ops_request_misc=&request_id=&biz_id=102&utm_term=c++线程池&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-2-90319537.142^v88^control_2,239^v2^insert_chatgpt&spm=1018.2226.3001.4187)

