#pragma once
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "z_msg_define.h"
#include <libgen.h>

//创建一个分离态的线程，设置线程属性
int start_detach_thread(pthread_t *thread, void *start_functio, void *arg);

//初始化头
struct msg_header get_init_header(unsigned short msk, int len);

