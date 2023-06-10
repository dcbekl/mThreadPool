#pragma once

/*
*   * 自定义异常类型
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

    char const* what() const;  // 获取具体的错误信息


private:
    T err_type;     // 异常类型
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

