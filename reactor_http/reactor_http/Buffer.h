#pragma once
struct Buffer
{
	//ָ���ڴ��ָ��
	char* data;
	int capacity;
	int readPos;
	int writePos; 
};

//��ʼ��
struct Buffer* bufferInit(int size);
//�ͷ��ڴ�
void bufferDestory(struct Buffer* buf);
//����
void bufferExtendRoom(struct Buffer* buffer, int size);
//�õ�ʣ���д���ڴ�����
int bufferWriteableSize(struct Buffer* buffer);
//�õ�ʣ��ɶ����ڴ�����
int bufferReadableSize(struct Buffer* buffer);
//д�ڴ� 1.ֱ��д2.�����׽���ͨ������
int bufferAppendData(struct Buffer* buffer, const char* data, int size);
int bufferAppendString(struct Buffer* buffer, const char* data);
int bufferSocketRead(struct Buffer* buffer, int fd);