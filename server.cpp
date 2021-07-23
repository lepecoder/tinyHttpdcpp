#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>

/******************************
 * 多进程版TCP服务端          *
 * 主进程负责监听客户端的请求 *
 * FORK子进程负责与客户端通信 *
 ******************************/

#define BUFFSIZE 2048
#define MAXLINK 2048
int sockfd; // 用于监听的套接字
uint32_t DEFAULT_PORT = 12343;
void stopServerRunning(int p) {
    printf("int p=%d, SIGINT=2    ", p);
    // 关闭服务器
    close(sockfd);
    printf("Close Server\n");
    exit(0);
}
// 信号处理函数
void callback(int num) {
    while (true) {
        pid_t pid = waitpid(-1, NULL, WNOHANG); // 等待匹配的进程死亡，-1表示匹配任何进程
        if (pid <= 0) {
            printf("子进程正在运行或回收完毕\n");
            break;
        }
        printf("child die, pid = %d\n", pid);
    }
}

int childWork(int cfd); // 声明子进程处理函数

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
    // 注册信号捕捉
    /* 子进程退出后会向父进程发送SIGCHLD信号，父进程通过sigaction捕获这个
    信号，通过回调函数callback()中的waitpid()对子进程的资源回收
    但SIGCHLD的子进程中断信号会中断accept()过程，因此需要在connfd中判断是由
    中断解除了阻塞还是其它错误。
     */
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = callback;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    char buff[BUFFSIZE]; // 用于收发数据
    printf("Listening...\n");
    while (true) {
        signal(SIGINT, stopruning); //在control-C的时候关闭服务器
        /* 4.阻塞等待客户端连接 */
        struct sockaddr_in connaddr;
        socklen_t addrlen = sizeof(connaddr);
        int connfd = accept(sockfd, (struct sockaddr *)&connaddr, &addrlen); // 后面的NULL保存了客户端的地址信息
        if (-1 == connfd) {
            if (errno == EINTR) {
                // accept被信号中断，导致解除了阻塞
                // 重新调用
                continue;
            }
            perror("Accept error");
            return -1;
        }
        // 创建子进程，和客户端通信
        pid_t pid = fork();
        if (pid == 0) {
            // 创建新的进程
            // 父进程中的资源被拷贝到了子进程
            // 子进程不负责监听，可以关闭监听套接字
            close(sockfd);
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
            exit(0); // 退出子进程
        } else if (pid > 0) {
            /* fork()函数被调用后会有两次返回，因为子进程和父进程会做相同的事情
            使用PID区分是子进程还是父进程，pid=0表示是子进程，pid>0表示是父进程
             */
            close(connfd); // 父进程关闭通信套接字，因为不需要
        } else {
            perror("fork() error");
            exit(0);
        }
    }
    close(sockfd);
    return 0;
}
