#pragma once
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "msg_define.h"
#include <libgen.h>
#include "upline_user_controller.h"

//设置线程属性
int start_detach_thread(pthread_t *thread, void *start_functio, void *arg);

//判断文件是否存在
int file_exist(const char *file_path);

//获取文件大小
unsigned long get_file_size(const char *path);

struct msg_header get_init_header(unsigned short msk, int len);

//获取文件的名字
char *get_file_name(const char *path);

//校验输入的发送人
int verify_input_send(const char *input_user);


void InputCode(char *pass);