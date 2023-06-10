#pragma once

/*
*   * �Զ����쳣����
*/

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

namespace ErrorType
{
    enum class SafeQueue
    {
        empty
    };
}


template<typename T>
class MTheadPoolError : public std::exception {
public:
    MTheadPoolError(T _err_type) : err_type(_err_type) {};

    char const* what() const;  // ��ȡ����Ĵ�����Ϣ


private:
    T err_type;     // �쳣����
    std::stringstream ss;
};


template<typename T>
char const* MTheadPoolError<T>::what() const
{
    switch (err_type)
    {
    case T::empty :
        return "SafeQueue can't pop when it's empty!";
        break;

    default:
        break;
    }
    return "None";
}

