#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include "Server.h"
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<string.h>
#include<strings.h>
#include <errno.h> // 包含 errno.h 头文件，以使用 EAGAIN 等错误码
#include<sys/stat.h>
int initListenFd(unsigned short port)
{
    //创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }
    //设置端口复用
    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (ret == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }
    //设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }
    //返回fd
    return lfd;
}

int epollRun(int lfd)
{
    //1.创建epoll实例
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll_create");
        return -1;
    }
    //2.lfd上树
    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    //3.检测
    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(struct epoll_event);
    while (1)
    {
        int num = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < num; i++)
        {
            int fd = evs[i].data.fd;
            if (fd == lfd)
            {
                //建立新连接 accept
                acceptClient(lfd, epfd);
            }
            else
            {
                //主要是接收对端的数据（读端）
                recvHttpRequest(fd, epfd);
            } 
        }
    }
    return 0;
}

int acceptClient(int lfd, int epfd)
{
    //1.建立连接
    int cfd = accept(lfd, NULL, NULL);
    if (cfd == -1)
    {
        perror("accept");
        return -1;
    }
    //2.修改默认属性为非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    //cfd添加到epoll中
    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;//设置边沿非阻塞模式
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    return 0;
}

int recvHttpRequest(int cfd, int epfd)
{
    int len = 0, totle = 0;
    //防止buf中数据被覆盖
    char tmp[1024] = { 0 };
    char buf[4096] = { 0 };
    while ((len = recv(cfd, buf + totle, sizeof buf - totle, 0)) > 0) // 修正接收数据的逻辑，从 totle 偏移位置开始接收，防止覆盖，同时修正括号使用
    {
        totle += len;
    }
    //判断数据是否接收完毕
    if (len == -1 && errno == EAGAIN) // 使用 errno 来判断错误状态
    {
        //解析请求行

    }
    else if (len == 0)
    {
        //客户端断开了连接
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
        close(cfd);
    }
    else
    {
        perror("recv");
    }
    return 0;
}

int parseRequestLine(const char* line, int cfd)
{
    //解析请求行 get /xxx/1.jpg http/1.1
    char method[12];
    char path[1024];
    sscanf(line, "%[^ ] %[^ ]", method, path);
    if (strcasecmp(method, "get") != 0)
    {
        return -1;
    }
    //处理客户端请求的静态资源（目录或文件）
    char* file = NULL;
    if (strcmp(path, "/") == 0)
    {
        file = './';
    }
    else
    {
        file = path + 1;
    }
    //获取文件属性
    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1)
    {
        //文件不存在 --回复404
    }
    if (S_ISDIR(st.st_mode))
    {
        //把这个目录的内容发送给客户端

    }
    else
    {
        //把文件内容发送给客户端

    }
    return 0;
}
