#pragma once
#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#define MSG_SEND_AUTO
struct TcpConnection
{
	struct EventLoop* evLoop;
	struct Channel* channel;
	struct Buffer* readBuf;
	struct Buffer* writeBuf;
	char name[32];
	//httpЭ��
	struct HttpRequest* request; 
	struct HttpResponse* response;
};

//��ʼ��
struct TcpConnection* tcpConnectionInit(int fd, struct EventLoop* evLoop); 
//
void TcpConnectionDestroy(struct TcpConnection* conn);