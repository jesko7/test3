#pragma once
#include "z_msg_define.h"
#include "z_client_list.h"
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include "z_common_utils.h"
#include "z_upline_user_service.h"

void send_msg_handle(int sockfd, const char *content);

void kill_recv(int sockfd,const char *content);