#pragma once
#include "z_block_server.h"
#include "z_client_list.h"
#include "z_common_utils.h"
#include "z_msg_define.h"
#include "z_upline_user_service.h"
#include "z_register_service.h"
#include "z_msg_send_service.h"
#include "z_chat_dao.h"
#include "z_file_send_service.h"
#include "z_send_file_list.h"

int z_start_server(const int port, const char *ip); //启动服务

void *zchat_server_controller(void *arg); //服务端客户端交互层

int verify_header(struct msg_header header); //校验

void process_client_msg(int sockfd, const char *content, unsigned short control_mask);