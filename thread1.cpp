#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#define MAX 100
// 全局变量
int number;

// 创建一把互斥锁
// 全局变量, 多个线程共享
std::mutex mtx;

// 线程处理函数
void funcA_num() {
    for (int i = 0; i < MAX; ++i) {
        // 如果线程A加锁成功, 不阻塞
        // 如果B加锁成功, 线程A阻塞
        mtx.lock();
        int cur = number;
        cur++;
        usleep(10);
        number = cur;
        mtx.unlock();
        printf("Thread A, id = %lu, number = %d\n", pthread_self(), number);
    }
}

void funcB_num() {
    for (int i = 0; i < MAX; ++i) {
        // a加锁成功, b线程访问这把锁的时候是锁定的
        // 线程B先阻塞, a线程解锁之后阻塞解除
        // 线程B加锁成功了
        mtx.lock();
        int cur = number;
        cur++;
        number = cur;
        mtx.unlock();
        printf("Thread B, id = %lu, number = %d\n", pthread_self(), number);
        usleep(5);
    }
}

int main(int argc, const char *argv[]) {
    std::thread t1(funcA_num), t2(funcB_num);

    // 阻塞，资源回收
    t1.join();
    t2.join();
    // 销毁互斥锁
    // 线程销毁之后, 再去释放互斥锁

    return 0;
}