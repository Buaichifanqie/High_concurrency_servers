#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include "Server.h"
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<fcntl.h>
int initListenFd(unsigned short port)
{
	//���������׽���
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1)
	{
		perror("socket");
		return -1;
	}
	//���ö˿ڸ���
	int opt = 1;
	int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (ret == -1)
	{
		perror("setsockopt");
		return -1;
	}
	//��
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
	//���ü���
	ret = listen(lfd, 128);
	if (ret == -1)
	{
		perror("listen");
		return -1;
	}
	//����fd
	return lfd;
}

int epollRun(int lfd)
{
	//1.����epollʵ��
	int epfd = epoll_create(1);
	if (epfd == -1)
	{
		perror("epoll_create");
		return -1;
	}
	//2.lfd����
	struct epoll_event ev;
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	if (ret == -1)
	{
		perror("epoll_ctl");
		return -1;
	}
	//3.���
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
				//���������� accept
				acceptClient(lfd, epfd);
			}
			else
			{
				//��Ҫ�ǽ��նԶ˵����ݣ����ˣ�

			}
		}
	}
	return 0;
}

int acceptClient(int lfd, int epfd)
{
	//1.��������
	int cfd = accept(lfd, NULL, NULL);
	if (cfd == -1)
	{
		perror("accept");
		return -1;
	}
	//2.�޸�Ĭ������Ϊ������
	int flag = fcntl(cfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(cfd, F_SETFL, flag);

	//cfd��ӵ�epoll��
	struct epoll_event ev;
	ev.data.fd = cfd;
	ev.events = EPOLLIN | EPOLLET;//���ñ��ط�����ģʽ
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
	if (ret == -1)
	{
		perror("epoll_ctl");
		return -1;
	}
	return 0;
}
