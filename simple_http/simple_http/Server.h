#pragma once
//初始化监听的套接字
int initListenFd();
//启动epoll
int epollRun(int lfd);
//和客户端建立连接
int acceptClient(int lfd,int epfd);
//接收http请求
int recvHttpRequest(int cfd, int epfd);
//解析请求行
int parseRequestLine(const char* line,int cfd);