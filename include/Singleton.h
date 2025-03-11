/**
 * @FilePath     : /mxChat/include/Singleton.h
 * @Description  :  单例模式实现
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:36:27
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <memory>
#include <mutex>
#include <iostream>

template <typename T>
/**
 * @author       : caomengxuan666
 * @brief        : 单例模式的封装类,采用CTPR模式
 * @return        {*}
**/
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;

    static std::shared_ptr<T> _instance;
public:
    /**
     * @author       : caomengxuan666
     * @brief        : 获取单例对象
     * @return        {*}
    **/
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = std::shared_ptr<T>(new T);
            });

        return _instance;
    }
    /**
     * @author       : caomengxuan666
     * @brief        : 打印单例对象的地址
     * @return        {*}
    **/    
    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }
    /**
     * @author       : caomengxuan666
     * @brief        : 单例类的析构函数
     * @return        {*}
    **/    
    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;