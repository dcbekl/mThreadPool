#pragma once

# include <iostream>
# include <queue>
# include <mutex>
# include "errors.h"

template <typename T>
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

	// 弹出队列中的一个元素
	T Pop();
};


template <typename T>
bool SafeQueue<T>::Empty()
{
	std::unique_lock<std::mutex> mlock(m_mutex);
	return m_queue.empty();
}


template <typename T>
int SafeQueue<T>::Size()
{
	std::unique_lock<std::mutex> mlock(m_mutex);
	return int(m_queue.size());
}


template <typename T>
void SafeQueue<T>::Push(const T t)
{
	std::unique_lock<std::mutex> mlock(m_mutex);
	m_queue.push(t);
}

template <typename T>
T SafeQueue<T>::Pop()
{
	std::unique_lock<std::mutex> mlock(m_mutex);
	if (m_queue.empty()) {
		throw MTheadPoolError<ErrorType::SafeQueue>(ErrorType::SafeQueue::empty);
		return T();
	}

	T t = m_queue.front();
	m_queue.pop();
	return std::move(t);
}