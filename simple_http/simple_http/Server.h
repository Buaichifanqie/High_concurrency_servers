#pragma once
//��ʼ���������׽���
int initListenFd();
//����epoll
int epollRun(int lfd);
//�Ϳͻ��˽�������
int acceptClient(int lfd,int epfd);
