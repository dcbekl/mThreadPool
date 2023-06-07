#pragma once

# include <iostream>
# include <queue>
# include <mutex>
# include "errors.h"

template <typename T>
class SafeQueue
{
private:
	std::queue<T> m_queue; // �������
	std::mutex m_mutex;	   // ������

public:
	SafeQueue()  {}	// ���޲ι��캯��
	~SafeQueue() {} // ����������

	// �ж϶����Ƿ�Ϊ��
	bool Empty();

	// ���ض�����Ԫ�صĸ���
	int Size();

	// �����������й���Ԫ��
	void Push (T t);

	// ���������е�һ��Ԫ��
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