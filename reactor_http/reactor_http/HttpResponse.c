#include "HttpResponse.h"
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#define ResHeaderSize 16
struct HttpResponse* httpResponseInit()
{
	struct HttpResponse* response = (struct HttpResponse*)malloc(sizeof(struct HttpResponse));
	response->headerNum = 0;
	int size = sizeof(struct ResponseHeader) * ResHeaderSize;
	response->headers = (struct ResponseHeader*)malloc(size);
	response->statusCode = Unknow;
	//��ʼ������
	bzero(response->headers, size);
	bzero(response->statusMsg, sizeof(response->statusMsg));
	bzero(response->filename, sizeof(response->filename));
	//����ָ��
	response->sendDataFunc = NULL;
	return response;
}

void httpResponseDestory(struct HttpResponse* response)
{
	if (response != NULL)
	{
		free(response->headers);
		free(response);
	}
}

void httpResponseAddHeader(struct HttpResponse* response, const char* key, const char* value)
{
	if (response == NULL || key == NULL || value == NULL)
	{
		return;
	}
	strcpy(response->headers[response->headerNum].key, key);
	strcpy(response->headers[response->headerNum].value, value);
	response->headerNum++;
}

void httpResponsePrepareMsg(struct HttpResponse* response,struct Buffer* sendBuf, int socket)
{
	//״̬��
	char tmp[1024] = { 0 };
	sprintf(tmp, "HTTP/1.1 %d %s\r\n", response->statusCode, response->statusMsg);
	bufferAppendString(sendBuf, tmp);
	//��Ӧͷ
	for (int i = 0; i < response->headerNum; ++i)
	{
		sprintf(tmp, "%s: %s\r\n", response->headers[i].key, response->headers[i].value);
		bufferAppendString(sendBuf, tmp);
	}
	//����
	bufferAppendString(sendBuf,"\r\n");
#ifndef MSG_SEND_AUTO
	bufferSendData(sendBuf, socket);
#endif	
	//�ظ�������
	response->sendDataFunc(response->filename, sendBuf, socket);
}
