#pragma once
#include<stdbool.h>

//�����ļ��������Ķ�д�¼��ã������ƣ�
//��10��д ��100���� ��110����д
enum FdEvent
{
	TimeOut = 0x01,//�Ƿ�ʱ
	ReadEvent = 0x02,
	WriteEvent = 0x04
};


//���庯��ָ��
typedef int(*handleFunc)(void* arg);

struct Channel
{
	//�ļ�������
	int fd;
	//�¼�
	int events;
	//�ص�����
	handleFunc readCallback;
	handleFunc writeCallback;
	handleFunc destroyCallback;
	//�ص������Ĳ���
	void* arg;
};
//��ʼ��һ��Channel
struct Channel* channelInit(int fd,int events,handleFunc readFunc,handleFunc writeFunc, handleFunc destroyFunc, void* arg);
//�޸�fd��д�¼������or����⣩
void writeEventEnable(struct Channel* channel, bool flag);
//�ж��Ƿ�Ҫ����ļ���������д�¼�
bool isWriteEventEanble(struct Channel* channel);






