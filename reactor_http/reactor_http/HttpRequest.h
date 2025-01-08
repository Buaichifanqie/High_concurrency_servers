#pragma once
#include "Buffer.h"
#include <stdbool.h>
//����ͷ��ֵ��
struct RequestHeader
{
	char* key;
	char* value;
};

//��ǰ������״̬
enum HttpRequestState
{
	ParseReqLine,
	ParseReqHeaders,
	ParseReqBody,
	ParseReqDone
};

//����http����ṹ��
struct HttpRequest
{
	char* method;
	char* url;
	char* version;
	struct RequestHeader* reqHeaders;
	int reqHeadersNum;
	enum HttpRequestState curState;
};
//��ʼ��
struct HttpRequest* httpRequestInit();
//����
void httpRequestReset(struct HttpRequest* req);
void httpRequestResetEx(struct HttpRequest* req);
void httpRequestDestroy(struct HttpRequest* req);
//��ȡ����״̬
enum HttpRequestState httpRequestState(struct HttpRequest* request);
//�������ͷ
void httpRequestAddHeader(struct HttpRequest* request, const char* key, const char* value);
//����key�õ�����ͷvalue
char* httpRequestGetHeader(struct HttpRequest* request, const char* key);
//����������
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf);
//��������ͷ
bool parseHttpRequestHeader(struct HttpRequest* request, struct Buffer* readBuf);
//����http����Э��
bool parseHttpRequest(struct HttpRequest* request, struct Buffer* readBuf);
//����http����Э��
bool processHttpRequest(struct HttpRequest* request);
//�����ַ���
void decodeMsg(char* to, char* from);
