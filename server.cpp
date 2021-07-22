#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/unistd.h>

#define BUFFSIZE 2048
// #define DEFAULT_PORT 12343
#define MAXLINK 2048
int sockfd, connfd; // 分别用于监听和通讯
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
    void (*stopruning)(int p); // 创建函数指针
    stopruning = stopServerRunning;
    /* 1.建立socket连接 */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        perror("create socket error");
        return -1;
    }
    /* 2.bind网络地址 */
    struct sockaddr_in servaddr;        // 用于存放IP和端口的结构
    bzero(&servaddr, sizeof(servaddr)); // 初始化置零
    servaddr.sin_family = AF_INET;
    // host to net long   主机字节序转网络字节序 32位整型
    // s_addr是个整型数，htonl可以把IP地址转成整型
    // 绑定0.0.0.0实际上是绑定本地网卡的IP地址
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 任何地址0.0.0.0
    // host to net short  16位整型
    servaddr.sin_port = htons(DEFAULT_PORT);
    if (-1 == bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
        perror("bind error");
        return -1;
    }
    /* 3.监听端口 */
    if (-1 == listen(sockfd, MAXLINK)) { // MAXLINK最大连接数量,之后的连接会排队等待
        perror("listen error");
        return -1;
    }
    char buff[BUFFSIZE]; // 用于收发数据
    printf("Listening...\n");
    while (true) {
        signal(SIGINT, stopruning); //在control-C的时候关闭服务器
        /* 4.阻塞等待客户端连接 */
        struct sockaddr_in connaddr;
        socklen_t addrlen = sizeof(connaddr);
        connfd = accept(sockfd, (struct sockaddr *)&connaddr, &addrlen); // 后面的NULL保存了客户端的地址信息
        if (-1 == connfd) {
            perror("Accept error");
            return -1;
        }
        while (true) {
            bzero(buff, BUFFSIZE);
            // 6. 接收信息
            int len = recv(connfd, buff, BUFFSIZE - 1, 0);
            char ip[32];
            inet_ntop(AF_INET, &connaddr.sin_addr.s_addr, ip, sizeof(ip));
            printf("client IP: %s, port: %d  ", ip, ntohs(connaddr.sin_port));
            if (len > 0) {
                printf("RECV: %s\n", buff);
                send(connfd, buff, strlen(buff), 0);
            } else if (len == 0) {
                printf("与客户端断开了连接\n");
                break;
            } else {
                perror("recv");
                break;
            }
            // 7. 关闭连接
        }
        close(connfd);
    }
    close(sockfd);
    return 0;
}
