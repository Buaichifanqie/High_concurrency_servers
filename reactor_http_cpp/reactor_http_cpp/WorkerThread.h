#pragma once
#include "EventLoop.h"
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

// 定义子线程对应的结构体
class WorkerThread
{
public:
    // 初始化
    WorkerThread(int index);
    ~WorkerThread();
    // 启动线程
    void run();
    inline EventLoop* getEventLoop()
    {
        return m_evLoop;
    }

private:
    void running();
private:
    thread* m_thread;//保存线程实例（保存线程地址的指针）
    thread::id m_threadID; // ID
    string m_name;
    mutex m_mutex;  // 互斥锁
    condition_variable m_cond;    // 条件变量
    EventLoop* m_evLoop;   // 反应堆模型
};

