#pragma once

#include <iostream>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <memory>
#include "SafeQueue.h"

#define MIN_POOL_SIZE 10                    /*线程池初始最少创建 10 个线程*/
const int DEFAULT_TIME = 300;               /*300ms检测一次*/
const int MIN_WAIT_TASK_NUM = 10;           /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/
const int DEFAULT_THREAD_VARY = 10;         /*每次创建和销毁线程的个数*/


// 线程池
/*  
*   对外提供的核心方法：
*       -- 线程池的创建
*           -- 线程池的大小
*       -- 线程中添加任务
*           -- 传入任务对象 仿函数
*       -- 线程池的启动
*       -- 线程池的关闭
*/

class mThreadPool
{
private:
    // 工作线程
    class mThreadWorker {//内置线程工作类

    private:
        int m_id; //工作id
        mThreadPool* m_pool;//所属线程池
        

    public:
        //构造函数
        mThreadWorker(mThreadPool* pool, const int id) : m_pool(pool), m_id(id) {}
        bool cont() { return (m_pool->m_shutdown || !m_pool->m_queue.Empty()); }

        //重载`()`操作
        void operator()() {
            while (!m_pool->m_shutdown || !(m_pool->m_queue).Empty())
            {
                {
                    std::cout << "in\n";
                    std::unique_lock<std::mutex> mlock(m_pool->m_mutex);

                    m_pool->free_thr_num++;
                    (m_pool->m_con).wait(mlock);
                    m_pool->free_thr_num--;
                    // 取任务
                    std::cout << "子线程 " << m_id << " 取出一个任务" << std::endl;
                    if (!(m_pool->m_queue).Empty()) {
                        auto task = static_cast<std::function<void()>>((m_pool->m_queue).Pop());
                        // 执行任务
                        task();
                    }
                }
            }
        }
        ~mThreadWorker() { std::cout << "子线程 " << m_id << " 关闭\n"; }
    };
public:
    // 创建线程池
    mThreadPool();
    mThreadPool(int _thr_num);
    ~mThreadPool();  // 退出线程池

    void start(); // 启动线程池
    void stop() { this->m_stop.store(true); } // 停止任务提交
    void restart() { this->m_stop.store(false); } // 重启任务提交
    void shutdown(); // 关闭线程池
    void addtask(std::function<void()> task); // 添加任务

private:
    // 管理线程
    /*
    * 负责从任务队列中取任务，控制执行
    *   -- 任务队列为空
    *   -- 任务队列非空，空闲工作线程为空
    */  
    class ManagerThread
    {
    private:
        mThreadPool* pool_ptr;
    public:
        ManagerThread(mThreadPool* _pool_ptr) : pool_ptr(_pool_ptr) { std::cout << "管理线程启动" << std::endl; }
        void operator()() {
            /*每个 DEFAULT_TIME/0.3 秒检查一次工作队列，如果工作队列不为空，唤醒等待的工作线程执行工作 */
            while (!(pool_ptr->m_shutdown.load()) || !pool_ptr->m_queue.Empty())
            {
                std::cout << "123";
                std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_TIME));
                if (!pool_ptr->m_queue.Empty())
                {
                    std::cout << "剩余工作量" << pool_ptr->m_queue.Size() << " 空闲线程数 " << pool_ptr->free_thr_num << std::endl;
                    pool_ptr->m_con.notify_all();
                }
            }
            pool_ptr->m_con.notify_all();
            for (int i = 0; i < pool_ptr->thr_num; ++i) { // 等待所有的工作线程结束，并释放线程
                if (pool_ptr->_m_pool[i]->joinable()) pool_ptr->_m_pool[i]->join();
            }
        }
        ~ManagerThread() { 
            std::cout << "管理线程关闭" << std::endl;
        }
        ManagerThread(const ManagerThread& other) { this->pool_ptr = other.pool_ptr; std::cout << "拷贝构造管理线程启动" << std::endl; };
        ManagerThread(ManagerThread&& other) noexcept { this->pool_ptr = other.pool_ptr; std::cout << "移动构造管理线程启动" << std::endl; };
    };
   
private:
    mThreadPool(const mThreadPool&) = delete; //拷贝构造函数，并且取消默认父类构造函数
    mThreadPool(mThreadPool&&) = delete; // 拷贝构造函数，允许右值引用
    mThreadPool& operator=(const mThreadPool&) = delete; // 赋值操作
    mThreadPool& operator=(mThreadPool&&) = delete; //赋值操作

public:
    int exc_num = 0;
    std::atomic<bool> m_stop;       // 是否关闭提交
    std::atomic<bool> m_shutdown;   // 是否关闭线程池
    
    int free_thr_num;               // 工作线程的个数 
    int thr_num;                    // 线程池线程数目

    std::mutex m_mutex;
    std::condition_variable m_con;  // 创建一个条件变量

    SafeQueue<std::function<void()>> m_queue;   // 任务队列
    std::vector<std::unique_ptr<std::thread>> _m_pool;
    std::unique_ptr<std::thread> ManagerThread_ptr;
};
