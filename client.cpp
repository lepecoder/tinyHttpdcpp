#include <arpa/inet.h>
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
#define SERVER_PORT 12343     // 指定服务端的port
int main() {
    struct sockaddr_in servaddr;
    char buff[BUFFSIZE];
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // presentation to numeric
    inet_pton(AF_INET, SERVER_IP,
              &servaddr.sin_addr); // 将点分十进制的ip地址转为数值
    servaddr.sin_port = htons(SERVER_PORT); // 网络相关的数字全都是网络字节顺序
    if (-1 == connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    printf("Please input: ");
    scanf("%s", buff);
    send(sockfd, buff, strlen(buff), 0);
    bzero(buff, sizeof(buff));
    recv(sockfd, buff, BUFFSIZE - 1, 0);
    printf("Recv: %s\n", buff);
    close(sockfd);
    return 0;
}