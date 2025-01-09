#pragma once
#include<stdbool.h>

//定义文件描述符的读写事件用（二进制）
//（10）写 （100）读 （110）读写
enum FdEvent
{
	TimeOut = 0x01,//是否超时
	ReadEvent = 0x02,
	WriteEvent = 0x04
};


//定义函数指针
typedef int(*handleFunc)(void* arg);

struct Channel
{
	//文件描述符
	int fd;
	//事件
	int events;
	//回调函数
	handleFunc readCallback;
	handleFunc writeCallback;
	handleFunc destroyCallback;
	//回调函数的参数
	void* arg;
};
//初始化一个Channel
struct Channel* channelInit(int fd,int events,handleFunc readFunc,handleFunc writeFunc, handleFunc destroyFunc, void* arg);
//修改fd的写事件（检测or不检测）
void writeEventEnable(struct Channel* channel, bool flag);
//判断是否要检测文件描述符的写事件
bool isWriteEventEanble(struct Channel* channel);






