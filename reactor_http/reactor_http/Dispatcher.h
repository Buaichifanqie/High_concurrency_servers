#pragma once

#include "Channel.h"
#include "EventLoop.h"
//���涼�Ǻ���ָ��
struct Dispatcher//�¼��ַ���
{
	//��ʼ��epoll poll select��Ҫ�����ݿ�
	void* (*init)();
	//����
	int(*add)(struct Channel* channel,struct EventLoop* evLoop); 
	//ɾ��
	int(*remove)(struct Channel* channel, struct EventLoop* evLoop);
	//�޸�
	int(*modify)(struct Channel* channel, struct EventLoop* evLoop);
	//�¼����
	int(*dispatch)(struct EventLoop* evLoop,int timeout);//��λ��s
	//������ݣ��ر�fd�����ͷ��ڴ棩
	int(*clear)(struct EventLoop* evLoop);
};