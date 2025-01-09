#pragma once
#include "EventLoop.h"
#include "ThreadPool.h"

struct Listener
{
	int lfd;
	unsigned short port;
};

struct TcpServer
{
	int threadNum;
	struct EventLoop* mainLoop;
	struct ThreadPool* threadPool;
	struct Listener* listener;
};

//��ʼ��
struct TcpServer* tcpServerInit(unsigned short port, int threadNum);
//��ʼ�����ڼ�����������
struct Listener* listenerInit(unsigned short port);
//����������
void TcpServerRun(struct TcpServer* server);
