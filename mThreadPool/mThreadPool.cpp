#include "mThreadPool.h"

// �����̳߳�
mThreadPool::mThreadPool() {
    thr_num = MIN_POOL_SIZE;
    free_thr_num = 0;
    m_stop.store(false);       
    m_shutdown.store(true);
}

// �����̳߳�
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
    puts("�̳߳عر�");
}


// �����̳߳أ����̳߳ؿ�ʼ����
void mThreadPool::start()
{
    puts("�̳߳ؿ�ʼ������");
    m_shutdown.store(false);
    for (int i = 0; i < thr_num; ++i) {;
        _m_pool.push_back(std::move(std::make_unique<std::thread>(mThreadWorker(this, i))));
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ManagerThread_ptr = std::move(std::make_unique<std::thread>(ManagerThread(this)));
}


// �ر��̳߳�
void mThreadPool::shutdown()
{
    // 1. ����̳߳��ѹر�
    if (m_shutdown) return; // ����̳߳��ѹرգ������
    
    // 2. �����̳߳عرձ�־Ϊ�棬׼���ر�
    m_shutdown.store(true);      // ���ùر��̱߳�־Ϊ �棬���ڻ������еȴ��߳�
    m_stop.store(true);          // ֹͣ�������������������߳̽���������е�����ִ������Զ��˳�

    // 3���رչ����߳�  -- �����߳��˳�ǰ��ر����еĹ����߳�
    ManagerThread_ptr->join();
}

// �������
void mThreadPool::addtask(std::function<void()> task)
{ 
    if (!m_stop) {
        m_queue.Push(task);
        std::cout << "���һ������" << exc_num+1 << std::endl;
        exc_num++;
        m_con.notify_one();
    }
}