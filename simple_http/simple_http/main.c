#include <stdio.h>
#include<unistd.h>
int main(int argc,int argv[])
{
    if (argc < 3)
    {
        printf("./a.out port path\n");
        return -1;
    }
    unsigned short port = atoi(argv[1]);
    //初始化用于监听的套接字
    int lfd = initListenFd(port);
    //切换服务器工作路径
    chdir(argv[2]);
    //启动服务器程序
    return 0;
}