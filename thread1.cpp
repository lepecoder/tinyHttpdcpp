#include <chrono>
#include <iostream>
#include <thread>
using namespace std;

void func(int num, string str) {
    for (int i = 0; i < 10; ++i) {
        cout << "子线程: i = " << i << "num: " << num << ", str: " << str << endl;
    }
}

void func1() {
    for (int i = 0; i < 10; ++i) {
        cout << "子线程: i = " << i << endl;
    }
}

int main() {
    cout << "主线程的线程ID: " << this_thread::get_id() << endl;
    thread t(func, 520, "i love you");
    thread t1(func1);
    cout << "线程t 的线程ID: " << t.get_id() << endl;
    cout << "线程t1的线程ID: " << t1.get_id() << endl;
    t.join();
    t1.join();
    return 0;
}
