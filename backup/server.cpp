#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>

#define BUFFSIZE 2048
// #define DEFAULT_PORT 12343
#define MAXLINK 2048
int sockfd, connfd; // 定义服务端套接字和客户端套接字
uint32_t DEFAULT_PORT = 12343;
void stopServerRunning(int p) {
    printf("int p=%d, SIGINT=2    ", p);
    // 关闭服务器
    close(sockfd);
    printf("Close Server\n");
    exit(0);
}

int main(int argc, char *arg[]) {
    if (argc > 1) {
        printf("%s\n", arg[1]);
        DEFAULT_PORT = atoi((arg[1]));
    }
    void (*stopruning)(int p);
    stopruning = stopServerRunning;
    struct sockaddr_in servaddr; // 用于存放IP和端口的结构
    char buff[BUFFSIZE];         // 用于收发数据
    // 1. 建立socket连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    // 2. bind网络地址
    bzero(&servaddr, sizeof(servaddr)); // 初始化置零
    servaddr.sin_family = AF_INET;
    // host to net long   主机字节序转网络字节序 32位整型
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 任何地址0.0.0.0
    // host to net short  16位整型
    servaddr.sin_port = htons(DEFAULT_PORT);
    if (-1 == bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
        printf("Bind error (%d): %s\n", errno, strerror(errno));
        return -1;
    }
    // 3.监听端口
    if (-1 == listen(sockfd, MAXLINK)) {
        printf("Listen error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    printf("Listening...\n");
    while (true) {
        signal(SIGINT, stopruning); //在control-C的时候关闭服务器
        connfd = accept(sockfd, NULL, NULL);
        if (-1 == connfd) {
            printf("Accept error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        bzero(buff, BUFFSIZE);
        // 6. 接收信息
        recv(connfd, buff, BUFFSIZE - 1, 0);
        printf("RECV: %s\n", buff);
        send(connfd, buff, strlen(buff), 0);
        // 7. 关闭连接
        close(connfd);
    }
    return 0;
}
