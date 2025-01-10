#define _CRT_SECURE_NO_WARNINGS
#include "Dispatcher.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define Max 520
struct EpollData
{
	int epfd;//epoll树的根节点
	struct epoll_event* events;//数组指针

};

static void* epollInit();
static int epollAdd(struct Channel* channel, struct EventLoop* evLoop);
static int epollRemove(struct Channel* channel, struct EventLoop* evLoop);
static int epollModify(struct Channel* channel, struct EventLoop* evLoop);
static int epollDispatch(struct EventLoop* evLoop, int timeout);//单位：s
static int epollClear(struct EventLoop* evLoop);

static int epollCtl(struct Channel* channel, struct EventLoop* evLoop, int op);


struct Dispatcher EpollDispatcher = {
	epollInit,
	epollAdd,
	epollRemove,
	epollModify,
	epollDispatch,
	epollClear
};

static void* epollInit()
{
	struct EpollData* data = (struct EpollData*)malloc(sizeof(struct EpollData));
	data->epfd = epoll_create(1);//参数大于0即可
	if (data->epfd == -1)
	{
		perror("epoll_create");
		exit(0);
	}
	data->events = (struct epoll_event*)calloc(Max, sizeof(struct epoll_event));

	return data;
}


static int epollCtl(struct Channel* channel, struct EventLoop* evLoop, int op)
{
	struct EpollData* data = (struct EpollData*)evLoop->dispatcherData;
	struct epoll_event ev;
	ev.data.fd = channel->fd;
	int events = 0;
	if (channel->events & ReadEvent)
	{
		events |= EPOLLIN;//通过标志位记录事件
	}
	if (channel->events & WriteEvent)//为什么不使用else if 因为有可能事件是既是读又是写事件
	{
		events |= EPOLLOUT;
	}
	ev.events = events;
	int ret = epoll_ctl(data->epfd, op, channel->fd, &ev);
	return ret;
}


static int epollAdd(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_ADD);
	if (ret == -1)
	{
		perror("epoll_ctl_add");
		exit(0);
	}
	return ret;
}
static int epollRemove(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
	if (ret == -1)
	{
		perror("epoll_ctl_del");
		exit(0);
	}
	//通过channel释放对应的TcpConnection资源
	channel->destroyCallback(channel->arg);
	return ret;
}
static int epollModify(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_MOD);
	if (ret == -1)
	{
		perror("epoll_ctl_mod");
		exit(0);
	}
	return ret;
}
static int epollDispatch(struct EventLoop* evLoop, int timeout)
{
	struct EpollData* data = (struct EpollData*)evLoop->dispatcherData;
	int count = epoll_wait(data->epfd, data->events, Max, timeout * 1000);
	for (int i = 0; i < count; ++i)
	{
		int events = data->events[i].events;
		int fd = data->events[i].data.fd;
		if (events & EPOLLERR || events & EPOLLHUP)
		{
			//对方断开了连接删除fd
			//epollRemove(Channel, evLoop);
			continue;
		}
		if (events & EPOLLIN)
		{
			eventActivate(evLoop, fd, ReadEvent);
		}
		if (events & EPOLLOUT)
		{
			eventActivate(evLoop, fd, WriteEvent);
		}
	}
	return 0;
}
static int epollClear(struct EventLoop* evLoop)
{
	struct EpollData* data = (struct EpollData*)evLoop->dispatcherData;
	free(data->events);
	close(data->epfd);
	free(data);
	return 0;
}
