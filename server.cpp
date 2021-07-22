#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

/*********************************************************
 * 服务端通信流程：                                         *
 * socket()->bind()->listen()->accept()->recv()->close() *
 *********************************************************/
