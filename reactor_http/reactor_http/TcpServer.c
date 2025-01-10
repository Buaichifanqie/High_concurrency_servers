#include "TcpServer.h"
#include <arpa/inet.h>
#include "TcpConnection.h"
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
struct TcpServer* tcpServerInit(unsigned short port, int threadNum)
{
	struct TcpServer* tcp = (struct TcpServer*)malloc(sizeof(struct TcpServer));
    tcp->listener = listenerInit(port);
	tcp->mainLoop = eventLoopInit();
	tcp->threadNum = threadNum;
	tcp->threadPool = threadPoolInit(tcp->mainLoop, threadNum);
	return tcp;
}

struct Listener* listenerInit(unsigned short port)
{
    //创建实例
    struct Listener* listener = (struct Listener*)malloc(sizeof(struct Listener));
    // 1. 创建监听的fd
    int lfd = socket(AF_INET, SOCK_STREAM, 0);//流式协议Tcp
    if (lfd == -1)
    {
        perror("socket");
        return NULL;
    }
    // 2. 设置端口复用
    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (ret == -1)
    {
        perror("setsockopt");
        return NULL;
    }
    // 3. 绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);//从小端转成大端
    addr.sin_addr.s_addr = INADDR_ANY;//零地址
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);
    if (ret == -1)
    {
        perror("bind");
        return NULL;
    }
    // 4. 设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return NULL;
    }
    // 返回listener
    listener->lfd = lfd;
    listener->port = port;
    return listener;
}


int acceptConnection(void* arg)
{
    struct TcpServer* server = (struct TcpServer*)arg;
    //和客户端建立连接
    int cfd = accept(server->listener->lfd, NULL, NULL);
    //从线程池中取出一个子线程的反应堆实例，去处理这个cfd
    struct EventLoop* evLoop = takeWorkeerEventLoop(server->threadPool);
    //将cfd添加到TcpConnection中处理
    tcpConnectionInit(cfd, evLoop);
    return 0;
}

void TcpServerRun(struct TcpServer* server)
{
    //启动线程池
    threadPoolRun(server->threadPool);
    //添加检测任务
    //初始化一个channel实例
    struct Channel* channel = channelInit(server->listener->lfd, ReadEvent, acceptConnection, NULL, NULL, server);
    eventLoopAddTask(server->mainLoop, channel, ADD);
    //启动反应堆模型
    eventLoopRun(server->mainLoop);
}
