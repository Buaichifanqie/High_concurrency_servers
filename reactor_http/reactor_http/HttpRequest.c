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
	char* tmp = (char*)malloc(length + 1);//尾部多存\0
	strncpy(tmp, start, length);
	tmp[length] = '\0';
	*ptr = tmp; 
	return space + 1;
}

bool parseHttpRequestLine(struct HttpRequest* request,struct Buffer* readBuf)
{
	//读出请求行,保存字符串的结束地址
	char* end = bufferFindCRLF(readBuf);
	//保存字符串起始地址
	char* start = readBuf->data + readBuf->readPos;
	//请求行总长度
	int lineSize = end - start;
	if (lineSize)
	{
		start = splitrRequestLine(start, end, " ", &request->method);
		start = splitrRequestLine(start, end, " ", &request->url);
		splitrRequestLine(start, end, NULL, &request->version);


#if 0
		//get /xxx/xx.txt http/1.1
		//请求方式
		char* space = memmem(start, lineSize, " ", 1);
		assert(space != NULL);
		int mothodSize = space - start;
		request->method = (char*)malloc(mothodSize + 1);//尾部多存\0
		strncpy(request->method, start, mothodSize);
		request->method[mothodSize] = '\0'; 

		//请求静态资源
		start = space + 1;
		char* space = memmem(start, end-start, " ", 1);
		assert(space != NULL);
		int urlSize = space - start;
		request->url = (char*)malloc(urlSize + 1);//尾部多存\0
		strncpy(request->url, start, urlSize);
		request->method[urlSize] = '\0';

		//请求http协议版本
		start = space + 1;
		//char* space = memmem(start, end - start, " ", 1);
		//assert(space != NULL);
		//int urlSize = space - start;
		request->version = (char*)malloc(end - start + 1);//尾部多存\0
		strncpy(request->url, start, end - start);
		request->method[end - start] = '\0';
#endif
		//为解析请求头做准备
		readBuf->readPos += lineSize;
		readBuf->readPos += 2;
		//修改状态
		request->curState = ParseReqHeaders;
		return true;
	}
	return false;
}

//该函数处理请求头的一行（多行多次调用）
bool parseHttpRequestHeader(struct HttpRequest* request,struct Buffer* readBuf)
{
	char* end = bufferFindCRLF(readBuf);
	if (end != NULL)
	{
		char* start = readBuf->data + readBuf->readPos;
		int lineSize = end - start;
		//基于搜索字符串
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
			//移动读数据的位置
			readBuf->readPos += lineSize;
			readBuf->readPos += 2;
		}
		else
		{
			//请求头被解析完了，跳过空行
			readBuf->readPos += 2;
			//修改解析状态
			//忽略post请求，按get请求处理
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
		//判断是否解析完毕了，如果完毕了，需要准备回复的数据
		if (request->curState == ParseReqDone)
		{
			//根据解析出的原始数据，对客户端的请求做出处理
			
			//组织响应数据并发送给客户端
		}
	}
	request->curState = ParseReqLine;//状态还原保证还能继续处理第二条 以及以后的请求
	return flag;
}

//处理基于get的http请求
bool processHttpRequest(struct HttpRequest* request)
{
	if (strcasecmp(request->method, "get") != 0)
	{
		return -1;
	}
	decodeMsg(request->url, request->url);
	// 处理客户端请求的静态资源(目录或者文件)
	char* file = NULL;
	if (strcmp(request->url, "/") == 0)
	{
		file = "./";
	}
	else
	{
		file = request->url + 1;
	}
	// 获取文件属性
	struct stat st;
	int ret = stat(file, &st);
	if (ret == -1)
	{
		// 文件不存在 -- 回复404
		//sendHeadMsg(cfd, 404, "Not Found", getFileType(".html"), -1);
		//sendFile("404.html", cfd);
		return 0;
	}
	// 判断文件类型
	if (S_ISDIR(st.st_mode))
	{
		// 把这个目录中的内容发送给客户端
		//sendHeadMsg(cfd, 200, "OK", getFileType(".html"), -1);
		//sendDir(file, cfd);
	}
	else
	{
		// 把文件的内容发送给客户端
		//sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
		//sendFile(file, cfd);
	}
	return false;
}

// 解码
// to 存储解码之后的数据, 传出参数, from被解码的数据, 传入参数
void decodeMsg(char* to, char* from)
{
	for (; *from != '\0'; ++to, ++from)
	{
		// isxdigit -> 判断字符是不是16进制格式, 取值在 0-f
		// Linux%E5%86%85%E6%A0%B8.jpg
		if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
		{
			// 将16进制的数 -> 十进制 将这个数值赋值给了字符 int -> char
			// B2 == 178
			// 将3个字符, 变成了一个字符, 这个字符就是原始数据
			*to = hexToDec(from[1]) * 16 + hexToDec(from[2]);

			// 跳过 from[1] 和 from[2] 因此在当前循环中已经处理过了
			from += 2;
		}
		else
		{
			// 字符拷贝, 赋值
			*to = *from;
		}

	}
	*to = '\0';
}
