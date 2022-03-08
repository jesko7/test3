#pragma once

#include<stdio.h>
#include<stdlib.h>
#include"common_utils.h"
#include"msg_define.h"
#include<string.h>
#include"z_block_server.h"
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
int z_chat_send_file(int sockfd);

int z_chat_recv_file_request(int sockfd,const char * content);

int z_chat_file_send_replay(int sockfd,const char* content);

int z_chat_refuse_recv_file(int sockfd);

int z_chat_confirm_recv_file(int oldfd);

void send_file_replay(int sockfd,unsigned short replay_msk,const char* err_msg);

int z_chat_confirm_send_file(int sockfd,const char * content);

void send_file_thread(void *arg);

void recv_file_thread(void *arg);