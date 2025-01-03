#pragma once

#include "Channel.h"
#include "EventLoop.h"
//里面都是函数指针
struct Dispatcher//事件分发器
{
	//初始化epoll poll select需要的数据块
	void* (*init)();
	//添加
	int(*add)(struct Channel* channel,struct EventLoop* evLoop); 
	//删除
	int(*remove)(struct Channel* channel, struct EventLoop* evLoop);
	//修改
	int(*modify)(struct Channel* channel, struct EventLoop* evLoop);
	//事件检测
	int(*dispatch)(struct EventLoop* evLoop,int timeout);//单位：s
	//清除数据（关闭fd或者释放内存）
	int(*clear)(struct EventLoop* evLoop);
};
