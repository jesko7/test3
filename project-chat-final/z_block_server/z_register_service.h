#pragma once

#include <stdio.h>
#include <string.h>
#include "z_msg_define.h"
#include "z_client_list.h"
#include <sys/socket.h>
#include <stdlib.h>
#include "z_upline_user_service.h"
#include "z_chat_dao.h"
#include <time.h>
#include <stddef.h>
#include "z_common_utils.h"
#include "z_send_file_list.h"

void register_handle(int sockfd, const char *content); // 注册

void user_login_handle(int sockfd, const char *content); // 登录

int user_is_exit_db(struct user_struct user); //判断注册用户是否在数据库中


int user_is_admin(struct user_struct user, unsigned short mask); //判断是否是admin用户