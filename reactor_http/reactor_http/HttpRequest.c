#include "HttpRequest.h"
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#define _CRT_SECURE_NO_WARNINGS

#define HeaderSize 12
struct HttpRequest* httpRequestInit()
{
	struct HttpRequest* request = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));
	httpRequestReset(request);
	request->reqHeaders = (struct RequestHeader*)malloc(sizeof(struct RequestHeader) * HeaderSize);
	return request;
}

void httpRequestReset(struct HttpRequest* req)
{
	req->curState = ParseReqLine;
	req->method = NULL;
	req->url = NULL;
	req->version = NULL;
	req->reqHeadersNum = 0;
}

void httpRequestResetEx(struct HttpRequest* req)
{
	free(req->url);
	free(req->method);
	free(req->version);
	if (req->reqHeaders != NULL)
	{
		for (int i = 0; i < req->reqHeadersNum; ++i)
		{
			free(req->reqHeaders[i].key);
			free(req->reqHeaders[i].value);
		}
		free(req->reqHeaders);
	}
	httpRequestReset(req);
}

void httpRequestDestroy(struct HttpRequest* req)
{
	if (req != NULL)
	{
		httpRequestResetEx(req);
		free(req);
	}
}

enum HttpRequestState httpRequestState(struct HttpRequest* request)
{
	return request->curState;
}

void httpRequestAddHeader(struct HttpRequest* request, const char* key, const char* value)
{
	request->reqHeaders[request->reqHeadersNum].key = key;
	request->reqHeaders[request->reqHeadersNum].value = value;
	request->reqHeadersNum++;
}

char* httpRequestGetHeader(struct HttpRequest* request, const char* key)
{
	if (request != NULL)
	{
		for (int i = 0; i < request->reqHeadersNum; ++i)
		{
			if (strncasecmp(request->reqHeaders[i].key, key, strlen(key)) == 0)
			{
				return request->reqHeaders[i].value;
			}
		}
	}
	return NULL;
}


char* splitrRequestLine(const char* start,const char* end,const char* sub,char** ptr)
{
	char* space = end;
	if (sub!=NULL)
	{
		space = memmem(start, end - start, sub, strlen(sub));
		assert(space != NULL);
	}
	int length = space - start;
	char* tmp = (char*)malloc(length + 1);//β�����\0
	strncpy(tmp, start, length);
	tmp[length] = '\0';
	*ptr = tmp; 
	return space + 1;
}

bool parseHttpRequestLine(struct HttpRequest* request,struct Buffer* readBuf)
{
	//����������,�����ַ����Ľ�����ַ
	char* end = bufferFindCRLF(readBuf);
	//�����ַ�����ʼ��ַ
	char* start = readBuf->data + readBuf->readPos;
	//�������ܳ���
	int lineSize = end - start;
	if (lineSize)
	{
		start = splitrRequestLine(start, end, " ", &request->method);
		start = splitrRequestLine(start, end, " ", &request->url);
		splitrRequestLine(start, end, NULL, &request->version);


#if 0
		//get /xxx/xx.txt http/1.1
		//����ʽ
		char* space = memmem(start, lineSize, " ", 1);
		assert(space != NULL);
		int mothodSize = space - start;
		request->method = (char*)malloc(mothodSize + 1);//β�����\0
		strncpy(request->method, start, mothodSize);
		request->method[mothodSize] = '\0'; 

		//����̬��Դ
		start = space + 1;
		char* space = memmem(start, end-start, " ", 1);
		assert(space != NULL);
		int urlSize = space - start;
		request->url = (char*)malloc(urlSize + 1);//β�����\0
		strncpy(request->url, start, urlSize);
		request->method[urlSize] = '\0';

		//����httpЭ��汾
		start = space + 1;
		//char* space = memmem(start, end - start, " ", 1);
		//assert(space != NULL);
		//int urlSize = space - start;
		request->version = (char*)malloc(end - start + 1);//β�����\0
		strncpy(request->url, start, end - start);
		request->method[end - start] = '\0';
#endif
		//Ϊ��������ͷ��׼��
		readBuf->readPos += lineSize;
		readBuf->readPos += 2;
		//�޸�״̬
		request->curState = ParseReqHeaders;
		return true;
	}
	return false;
}

//�ú�����������ͷ��һ�У����ж�ε��ã�
bool parseHttpRequestHeader(struct HttpRequest* request,struct Buffer* readBuf)
{
	char* end = bufferFindCRLF(readBuf);
	if (end != NULL)
	{
		char* start = readBuf->data + readBuf->readPos;
		int lineSize = end - start;
		//���������ַ���
		char* middle = memmem(start, lineSize, ": ", 2);
		if (middle != NULL)
		{
			char* key = malloc(middle - start + 1);
			strncpy(key, start, middle - start);
			key[middle - start] = '\0';

			char* value = malloc(end - middle - 2 + 1);
			strncpy(value, middle + 2, end - middle - 2);
			value[end - middle - 2] = '\0';

			httpRequestAddHeader(request, key, value);
			//�ƶ������ݵ�λ��
			readBuf->readPos += lineSize;
			readBuf->readPos += 2;
		}
		else
		{
			//����ͷ���������ˣ���������
			readBuf->readPos += 2;
			//�޸Ľ���״̬
			//����post���󣬰�get������
			request->curState = ParseReqDone;
		}
		return true;
	}

	return false;
}

bool parseHttpRequest(struct HttpRequest* request,struct Buffer* readBuf)
{
	bool flag = true;
	while (request->curState!=ParseReqDone)
	{
		switch (request->curState)
		{
		case ParseReqLine:
			flag=parseHttpRequestLine(request, readBuf);
			break;
		case ParseReqHeaders:
			flag=parseHttpRequestHeader(request, readBuf);
			break;
		case ParseReqBody:
			break;
		default:
			break;
		}
		if (flag)
		{
			return flag;
		}
		//�ж��Ƿ��������ˣ��������ˣ���Ҫ׼���ظ�������
		if (request->curState == ParseReqDone)
		{
			//���ݽ�������ԭʼ���ݣ��Կͻ��˵�������������
			
			//��֯��Ӧ���ݲ����͸��ͻ���
		}
	}
	request->curState = ParseReqLine;//״̬��ԭ��֤���ܼ�������ڶ��� �Լ��Ժ������
	return flag;
}

//�������get��http����
bool processHttpRequest(struct HttpRequest* request)
{
	if (strcasecmp(request->method, "get") != 0)
	{
		return -1;
	}
	decodeMsg(request->url, request->url);
	// ����ͻ�������ľ�̬��Դ(Ŀ¼�����ļ�)
	char* file = NULL;
	if (strcmp(request->url, "/") == 0)
	{
		file = "./";
	}
	else
	{
		file = request->url + 1;
	}
	// ��ȡ�ļ�����
	struct stat st;
	int ret = stat(file, &st);
	if (ret == -1)
	{
		// �ļ������� -- �ظ�404
		//sendHeadMsg(cfd, 404, "Not Found", getFileType(".html"), -1);
		//sendFile("404.html", cfd);
		return 0;
	}
	// �ж��ļ�����
	if (S_ISDIR(st.st_mode))
	{
		// �����Ŀ¼�е����ݷ��͸��ͻ���
		//sendHeadMsg(cfd, 200, "OK", getFileType(".html"), -1);
		//sendDir(file, cfd);
	}
	else
	{
		// ���ļ������ݷ��͸��ͻ���
		//sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
		//sendFile(file, cfd);
	}
	return false;
}

// ����
// to �洢����֮�������, ��������, from�����������, �������
void decodeMsg(char* to, char* from)
{
	for (; *from != '\0'; ++to, ++from)
	{
		// isxdigit -> �ж��ַ��ǲ���16���Ƹ�ʽ, ȡֵ�� 0-f
		// Linux%E5%86%85%E6%A0%B8.jpg
		if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
		{
			// ��16���Ƶ��� -> ʮ���� �������ֵ��ֵ�����ַ� int -> char
			// B2 == 178
			// ��3���ַ�, �����һ���ַ�, ����ַ�����ԭʼ����
			*to = hexToDec(from[1]) * 16 + hexToDec(from[2]);

			// ���� from[1] �� from[2] ����ڵ�ǰѭ�����Ѿ��������
			from += 2;
		}
		else
		{
			// �ַ�����, ��ֵ
			*to = *from;
		}

	}
	*to = '\0';
}
