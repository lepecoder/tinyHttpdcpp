#include <arpa/inet.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>
#define BUFFSIZE 2048
#define SERVER_IP "127.0.0.1" // 指定服务端的IP
#define SERVER_PORT 12345     // 指定服务端的port
int sockfd;                   // 与服务端通信的套接字
void stopServerRunning(int p) {
    printf("int p=%d, SIGINT=2    ", p);
    // 关闭服务器
    close(sockfd);
    printf("Close Server\n");
    exit(0);
}

int main() {
    struct sockaddr_in servaddr;
    char buff[BUFFSIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // 客户端通信套接字
    if (-1 == sockfd) {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);                   // 网络相关的数字全都是网络字节顺序
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr.s_addr); // 将点分十进制的ip地址转为数值
    if (-1 == connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    while (true) {
        signal(SIGINT, stopServerRunning); //在control-C的时候关闭服务器
        printf("Please input: ");
        scanf("%s", buff);
        send(sockfd, buff, strlen(buff) + 1, 0); // 长度最后有个\0
        bzero(buff, sizeof(buff));
        int len = recv(sockfd, buff, BUFFSIZE - 1, 0);
        if (len > 0) {
            printf("Recv: %s\n", buff);
        } else if (len == 0) {
            printf("与服务器断开了连接\n");
            break;
        } else {
            perror("RECV");
            break;
        }
    }
    close(sockfd);
    return 0;
}