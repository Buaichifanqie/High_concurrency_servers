#include "EventLoop.h"
#include<assert.h>
#define _CRT_SECURE_NO_WARNINGS

struct EventLoop* eventLoopInit()
{
	return eventLoopInitEx(NULL);
}

struct EventLoop* eventLoopInitEx(const char* threadName)
{
	struct EventLoop* evLoop = (struct EventLoop*)malloc(sizeof(struct EventLoop));
	evLoop->isQuit = false;
	evLoop->threadID = pthread_self();
	pthread_mutex_init(&evLoop->mutex, NULL);
	strcpy(evLoop->threadName , threadName == NULL ? "MainThread" : threadName);
	evLoop->dispatcher = &EpollDispatcher;
	evLoop->dispatcherData = evLoop->dispatcher->init();
	//����
	evLoop->head = evLoop->tail = NULL;
	//map
	evLoop->channelMap = channelMapInit(128);
	return evLoop;
}

int eventLoopRun(struct EventLoop* evLoop)
{
	assert(evLoop != NULL);
	//ȡ���¼��ַ��ͼ��ģ��
	struct Dispatcher* dispatcher = evLoop->dispatcher;
	//�Ƚ��߳�id�Ƿ�����
	if (evLoop->threadID == pthread_self())
	{
		return -1;
	}

	//ѭ���¼�����
	while (!evLoop->isQuit)
	{
		dispatcher->dispatch(evLoop, 2);
	}
	return 0;
}
