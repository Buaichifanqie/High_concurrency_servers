#pragma once
//��ʼ���������׽���
int initListenFd();
//����epoll
int epollRun(int lfd);
//�Ϳͻ��˽�������
int acceptClient(int lfd,int epfd);
//����http����
int recvHttpRequest(int cfd, int epfd);
//����������
int parseRequestLine(const char* line,int cfd);