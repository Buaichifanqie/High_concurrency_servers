#include "TcpConnection.h"
#include "HttpRequest.h"
#define _CRT_SECURE_NO_WARNINGS

int processRead(void* arg)
{
	struct TcpConnection* conn = (struct TcpConnection*)arg;
	//��������
	int count = bufferSocketRead(conn->readBuf, conn->channel->fd);
	if (count > 0)
	{
		//���յ���http���󣬽���http����
		int socket = conn->channel->fd;

#ifdef MSG_SEND_AUTO
		writeEventEnable(conn->channel, true);
		eventLoopAddTask(conn->evLoop, conn->channel, MODIFY);
#endif
		bool flag=parseHttpRequest(conn->request, conn->readBuf, conn->response, conn->writeBuf, socket);
		if (!flag)
		{
			//����ʧ�� �ظ�һ���򵥵�html
			char* errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
			bufferAppendString(conn->writeBuf, errMsg);
		}
	}
#ifndef MSG_SEND_AUTO
	//�Ͽ�����
	eventLoopAddTask(conn->evLoop, conn->channel, DELETE);
#endif
} 

int processWrite(void* arg)
{
	struct TcpConnection* conn = (struct TcpConnection*)arg;
	//��������
	int count = bufferSendData(conn->writeBuf, conn->channel->fd);
	if (count > 0)
	{
		//�ж������Ƿ���ȫ���ͳ�ȥ
		if (bufferReadableSize(conn->writeBuf) == 0)
		{
			//1.���ü��д�¼�--�޸�channel�б�����¼�
			writeEventEnable(conn->channel, false);
			//2.�޸�di'spatcher���ļ���--�������ڵ�
			eventLoopAddTask(conn->evLoop, conn->channel, MODIFY);
			//3.ɾ������ڵ�
			eventLoopAddTask(conn->evLoop, conn->channel, DELETE);
		}
	}
}

struct TcpConnection* tcpConnectionInit(int fd,struct EventLoop* evLoop)
{
	struct TcpConnection* conn = (struct TcpConnection*)malloc(sizeof(struct TcpConnection));
	conn->evLoop = evLoop;
	conn->readBuf = bufferInit(10240);
	conn->writeBuf = bufferInit(10240);
	//http
	conn->request = httpRequestInit();
	conn->response = httpResponseInit();
	sprintf(conn->name, "Connection-%d", fd);
	conn->channel = channelInit(fd, ReadEvent, processRead, processWrite,TcpConnectionDestroy, conn);
	eventLoopAddTask(evLoop, conn->channel, ADD);
	return conn;
}

int TcpConnectionDestroy(void* arg)
{
	struct TcpConnection* conn = (struct TcpConnection*)arg;
	if (conn != NULL)
	{
		if (conn->readBuf && bufferReadableSize(conn->readBuf) == 0 &&
			conn->writeBuf && bufferWriteableSize(conn->writeBuf) == 0)
		{
			destoryChannel(conn->evLoop, conn->channel);
			bufferDestory(conn->readBuf);
			bufferDestory(conn->writeBuf);
			httpRequestDestroy(conn->request);
			httpResponseDestory(conn->response);
			free(conn);
		}
	}
}
