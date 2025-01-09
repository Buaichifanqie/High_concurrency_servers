#pragma once
#include "Buffer.h"
//����״̬��ö��
enum HttoStatusCode
{
	Unknow,
	OK = 200,
	MovedPermanently = 301,
	MovedTemporarily = 302,
	BadRequest = 400,
	NotFound = 404
};

//������Ӧ�ṹ��
struct ResponseHeader
{
	char key[32];
	char value[128];
};

//����һ������ָ�� ������֯Ҫ�ظ����ͻ��˵����ݿ�
typedef void(*responseBody)(const char* fileName, struct Buffer* sendBuf, int socket);

//����ṹ��
struct HttpResponse
{
	//״̬�У�״̬�룬״̬����
	enum HttoStatusCode statusCode;
	char statusMsg[128];
	char filename[128];
	//��Ӧͷ-��ֵ��
	struct ResponseHeader* headers;
	int headerNum;
	responseBody sendDataFunc;
};

//��ʼ��
struct HttpResponse* httpResponseInit();
//����
void httpResponseDestory(struct HttpResponse* response);
//������Ӧͷ
void httpResponseAddHeader(struct HttpResponse*, const char* key, const char* value);
//��֯http��Ӧ����
void httpResponsePrepareMsg(struct HttpResponse* response, struct Buffer* sendBuf, int socket); 