#pragma once
#include "Dispatcher.h"
#include<stdbool.h>
#include "ChannelMap.h"
#include <pthread.h>
extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;

// 如何处理节点中的channel的方式
enum ElemType{ADD,DELECT,MODIFY};

//定义任务队列的节点
struct ChannelElement
{
	int type;//如何处理节点中的channel
	struct Channel* channel;
	struct ChannelElement* next;
};


struct EventLoop
{
	bool isQuit;
	struct Dispatcher* dispatcher;
	void* dispatcherData;

	//任务队列
	struct ChannelElement* head;
	struct ChannelElement* tail;
	//map
	struct ChannelMap* channelMap;
	//线程id,name,mutex
	pthread_t threadID;
	char threadName[32];
	pthread_mutex_t mutex;
	 
};

//初始化
struct EventLoop* eventLoopInit();//主
struct EventLoop* eventLoopInitEx(const char* threadName);//子

//启动反应堆模型
int eventLoopRun(struct EventLoop* evLoop);