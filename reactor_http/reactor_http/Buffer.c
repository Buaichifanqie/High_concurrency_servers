#include "Buffer.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/uio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h> 
#include<strings.h>
#define _CRT_SECURE_NO_WARNINGS

struct Buffer* bufferInit(int size)
{
	struct Buffer* buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
	if (buffer != NULL)
	{
		buffer->data = (char*)malloc(size);
		buffer->capacity = size;
		buffer->readPos = buffer->writePos = 0;
		memset(buffer->data, 0, size);
	}

	return buffer;
}

void bufferDestory(struct Buffer* buf)
{
	if (buf != NULL)
	{
		if (buf->data != NULL)
		{
			free(buf->data);
		}
	}
	free(buf);
}

void bufferExtendRoom(struct Buffer* buffer, int size)
{
	//1.�ڴ湻��--��������
	if (bufferWriteableSize(buffer) >= size)
	{
		return 0;
	}
	//2.�ڴ�ϲ��Ź���-��������
	//ʣ���д��+�Ѿ�����>=size
	else if (buffer->readPos + bufferWriteableSize(buffer) >= size)
	{
	    //�õ�δ���ڴ�Ĵ�С
		int readable = bufferReadableSize(buffer);
		//�ƶ��ڴ�
		memcpy(buffer->data, buffer->data + buffer->readPos, readable);
		//����λ��
		buffer->readPos = 0;
		buffer->writePos = readable;
	}
	//3.�ڴ治����-��Ҫ����
	else
	{
		void* temp = realloc(buffer->data, buffer->capacity + size);
		if (temp == NULL)
		{
			return; //ʧ����
		}
		memset(temp + buffer->capacity, 0, size);
		//��������
		buffer->data = temp;
		buffer->capacity += size;
	}
}

int bufferWriteableSize(struct Buffer* buffer)
{
	return buffer->capacity-buffer->writePos;
}

int bufferReadableSize(struct Buffer* buffer)
{
	return buffer->writePos-buffer->readPos;
}

int bufferAppendData(struct Buffer* buffer, const char* data, int size)
{
	if (buffer == NULL || data == NULL || data < 0)
	{
		return-1;
	}
	//����
	bufferExtendRoom(buffer, size); 
	//���ݿ���
	memcpy(buffer->data + buffer->writePos, data, size);
	return 0;
}

int bufferAppendString(struct Buffer* buffer, const char* data)
{
	int size = strlen(data);
	int ret = bufferAppendData(buffer, data, size);
	return ret;
}

int bufferSocketRead(struct Buffer* buffer, int fd)
{
	//read/recv/readv
	struct iovec vec[2];
	//��ʼ������Ԫ��
	int writeable = bufferWriteableSize(buffer);
	vec[0].iov_base = buffer->data + buffer->writePos;
	vec[0].iov_len = writeable;
	char* tmpbuf = (char*)malloc(40960);
	vec[1].iov_base = tmpbuf;
	vec[1].iov_len =40960;
	int result = readv(fd, vec, 2);
	if (result == -1)
	{
		return -1;
	}
	else if (result <= writeable)
	{
		buffer->writePos += result;
	}
	else
	{
		buffer->writePos = buffer->capacity;
		bufferAppendData(buffer, tmpbuf, result - writeable);
	}
	free(tmpbuf);
	return result;
}

char* bufferFindCRLF(struct Buffer* buffer)
{
	//strstr-->���ַ�����ƥ��С�ַ���������\0������
	//memmem-->�����ݿ���ƥ�������ݿ飨��Ҫָ�����ݿ��С��

	char* ptr = memmem(buffer->data + buffer->readPos, bufferReadableSize(buffer), "\r\n", 2);
	return ptr;
}

int bufferSendData(struct Buffer* buffer, int socket)
{
	//�ж���������
	int readable = bufferReadableSize(buffer);
	if (readable > 0)
	{
		int count = send(socket, buffer->data + buffer->readPos, readable,0);
		if (count > 0)
		{
			buffer->readPos += count;
			usleep(1);
		}
		return count;
	}
	return 0;
}
