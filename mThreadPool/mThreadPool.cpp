#include "mThreadPool.h"

// 创建线程池
mThreadPool::mThreadPool() {
    thr_num = MIN_POOL_SIZE;
    free_thr_num = 0;
    m_stop.store(false);       
    m_shutdown.store(true);
}

// 创建线程池
mThreadPool::mThreadPool(int _thr_num) {
    thr_num = (_thr_num > MIN_POOL_SIZE) ? _thr_num : MIN_POOL_SIZE;
    free_thr_num = 0;
    m_stop.store(false);
    m_shutdown.store(true);
}


mThreadPool::~mThreadPool()
{
    if (!m_shutdown)
        shutdown();
    puts("线程池关闭");
}


// 启动线程池，让线程池开始工作
void mThreadPool::start()
{
    puts("线程池开始工作！");
    m_shutdown.store(false);
    for (int i = 0; i < thr_num; ++i) {;
        _m_pool.push_back(std::move(std::make_unique<std::thread>(mThreadWorker(this, i))));
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ManagerThread_ptr = std::move(std::make_unique<std::thread>(ManagerThread(this)));
}


// 关闭线程池
void mThreadPool::shutdown()
{
    // 1. 如果线程池已关闭
    if (m_shutdown) return; // 如果线程池已关闭，则结束
    
    // 2. 设置线程池关闭标志为真，准备关闭
    m_shutdown.store(true);      // 设置关闭线程标志为 真，便于唤醒所有等待线程
    m_stop.store(true);          // 停止向任务队列中添加任务，线程将任务队列中的任务执行完后，自动退出

    // 3、关闭管理线程  -- 管理线程退出前会关闭所有的工作线程
    ManagerThread_ptr->join();
}

// 添加任务
void mThreadPool::addtask(std::function<void()> task)
{ 
    if (!m_stop) {
        m_queue.Push(task);
        std::cout << "添加一个任务" << exc_num+1 << std::endl;
        exc_num++;
        m_con.notify_one();
    }
}