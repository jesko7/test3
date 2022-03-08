#pragma once

#include<stdio.h>
#include"z_block_server.h"
#include"msg_define.h"
#include"register_controller.h"
#include"upline_user_controller.h"
#include"common_utils.h"
#include<error.h>
#include"msg_controller.h"
#include"z_chat_file_controller.h"

void z_chat_client_controller(const int port,const char* ip);

void z_chat_server_msg_handle_thread(void *arg);

void process_server_msg(int sockfd,const char* content,unsigned short control_mask);

void z_chat_client_send_msg(int sockfd,int is_send_all);

void kill(int sockfd);