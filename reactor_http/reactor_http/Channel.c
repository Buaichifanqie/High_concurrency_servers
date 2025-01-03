#include "Channel.h"

#define _CRT_SECURE_NO_WARNINGS

struct Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
	struct Channel* channel = (struct Channel*)malloc(sizeof(struct Channel));
	channel->arg = arg;
	channel->fd = fd;
	channel->events = events;
	channel->readCallback = readFunc;
	channel->writeCallback = writeFunc;
	return channel;
}

void writeEventEnable(struct Channel* channel, bool flag)
{
	if (flag)
	{
		channel->events |= WriteEvent;
	}
	else
	{
		//����
		channel->events = channel->events & ~WriteEvent;
	}
}

bool isWriteEventEanble(struct Channel* channel)
{
	return channel->events & WriteEvent;
}
