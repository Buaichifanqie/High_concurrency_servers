#pragma once
#include "Dispatcher.h"
#include<stdbool.h>
#include "ChannelMap.h"
#include <pthread.h>
extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;

// ��δ���ڵ��е�channel�ķ�ʽ
enum ElemType{ADD,DELECT,MODIFY};

//����������еĽڵ�
struct ChannelElement
{
	int type;//��δ���ڵ��е�channel
	struct Channel* channel;
	struct ChannelElement* next;
};


struct EventLoop
{
	bool isQuit;
	struct Dispatcher* dispatcher;
	void* dispatcherData;

	//�������
	struct ChannelElement* head;
	struct ChannelElement* tail;
	//map
	struct ChannelMap* channelMap;
	//�߳�id,name,mutex
	pthread_t threadID;
	char threadName[32];
	pthread_mutex_t mutex;
	 
};

//��ʼ��
struct EventLoop* eventLoopInit();//��
struct EventLoop* eventLoopInitEx(const char* threadName);//��

//������Ӧ��ģ��
int eventLoopRun(struct EventLoop* evLoop);