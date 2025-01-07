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

//初始化
struct TcpServer* tcpServerInit(unsigned short port, int threadNum);
//初始化用于监听的描述符
struct Listener* listenerInit(unsigned short port);
//启动服务器
void TcpServerRun(struct TcpServer* server);

