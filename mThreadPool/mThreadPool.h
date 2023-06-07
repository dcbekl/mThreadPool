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

#define MIN_POOL_SIZE 10                    /*�̳߳س�ʼ���ٴ��� 10 ���߳�*/
const int DEFAULT_TIME = 300;               /*300ms���һ��*/
const int MIN_WAIT_TASK_NUM = 10;           /*���queue_size > MIN_WAIT_TASK_NUM ����µ��̵߳��̳߳�*/
const int DEFAULT_THREAD_VARY = 10;         /*ÿ�δ����������̵߳ĸ���*/


// �̳߳�
/*  
*   �����ṩ�ĺ��ķ�����
*       -- �̳߳صĴ���
*           -- �̳߳صĴ�С
*       -- �߳����������
*           -- ����������� �º���
*       -- �̳߳ص�����
*       -- �̳߳صĹر�
*/

class mThreadPool
{
private:
    // �����߳�
    class mThreadWorker {//�����̹߳�����

    private:
        int m_id; //����id
        mThreadPool* m_pool;//�����̳߳�
        

    public:
        //���캯��
        mThreadWorker(mThreadPool* pool, const int id) : m_pool(pool), m_id(id) {}
        bool cont() { return (m_pool->m_shutdown || !m_pool->m_queue.Empty()); }

        //����`()`����
        void operator()() {
            while (!m_pool->m_shutdown || !(m_pool->m_queue).Empty())
            {
                {
                    std::cout << "in\n";
                    std::unique_lock<std::mutex> mlock(m_pool->m_mutex);

                    m_pool->free_thr_num++;
                    (m_pool->m_con).wait(mlock);
                    m_pool->free_thr_num--;
                    // ȡ����
                    std::cout << "���߳� " << m_id << " ȡ��һ������" << std::endl;
                    if (!(m_pool->m_queue).Empty()) {
                        auto task = static_cast<std::function<void()>>((m_pool->m_queue).Pop());
                        // ִ������
                        task();
                    }
                }
            }
        }
        ~mThreadWorker() { std::cout << "���߳� " << m_id << " �ر�\n"; }
    };
public:
    // �����̳߳�
    mThreadPool();
    mThreadPool(int _thr_num);
    ~mThreadPool();  // �˳��̳߳�

    void start(); // �����̳߳�
    void stop() { this->m_stop.store(true); } // ֹͣ�����ύ
    void restart() { this->m_stop.store(false); } // ���������ύ
    void shutdown(); // �ر��̳߳�
    void addtask(std::function<void()> task); // �������

private:
    // �����߳�
    /*
    * ��������������ȡ���񣬿���ִ��
    *   -- �������Ϊ��
    *   -- ������зǿգ����й����߳�Ϊ��
    */  
    class ManagerThread
    {
    private:
        mThreadPool* pool_ptr;
    public:
        ManagerThread(mThreadPool* _pool_ptr) : pool_ptr(_pool_ptr) { std::cout << "�����߳�����" << std::endl; }
        void operator()() {
            /*ÿ�� DEFAULT_TIME/0.3 ����һ�ι������У�����������в�Ϊ�գ����ѵȴ��Ĺ����߳�ִ�й��� */
            while (!(pool_ptr->m_shutdown.load()) || !pool_ptr->m_queue.Empty())
            {
                std::cout << "123";
                std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_TIME));
                if (!pool_ptr->m_queue.Empty())
                {
                    std::cout << "ʣ�๤����" << pool_ptr->m_queue.Size() << " �����߳��� " << pool_ptr->free_thr_num << std::endl;
                    pool_ptr->m_con.notify_all();
                }
            }
            pool_ptr->m_con.notify_all();
            for (int i = 0; i < pool_ptr->thr_num; ++i) { // �ȴ����еĹ����߳̽��������ͷ��߳�
                if (pool_ptr->_m_pool[i]->joinable()) pool_ptr->_m_pool[i]->join();
            }
        }
        ~ManagerThread() { 
            std::cout << "�����̹߳ر�" << std::endl;
        }
        ManagerThread(const ManagerThread& other) { this->pool_ptr = other.pool_ptr; std::cout << "������������߳�����" << std::endl; };
        ManagerThread(ManagerThread&& other) noexcept { this->pool_ptr = other.pool_ptr; std::cout << "�ƶ���������߳�����" << std::endl; };
    };
   
private:
    mThreadPool(const mThreadPool&) = delete; //�������캯��������ȡ��Ĭ�ϸ��๹�캯��
    mThreadPool(mThreadPool&&) = delete; // �������캯����������ֵ����
    mThreadPool& operator=(const mThreadPool&) = delete; // ��ֵ����
    mThreadPool& operator=(mThreadPool&&) = delete; //��ֵ����

public:
    int exc_num = 0;
    std::atomic<bool> m_stop;       // �Ƿ�ر��ύ
    std::atomic<bool> m_shutdown;   // �Ƿ�ر��̳߳�
    
    int free_thr_num;               // �����̵߳ĸ��� 
    int thr_num;                    // �̳߳��߳���Ŀ

    std::mutex m_mutex;
    std::condition_variable m_con;  // ����һ����������

    SafeQueue<std::function<void()>> m_queue;   // �������
    std::vector<std::unique_ptr<std::thread>> _m_pool;
    std::unique_ptr<std::thread> ManagerThread_ptr;
};
