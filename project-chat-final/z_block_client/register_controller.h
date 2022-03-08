#pragma once

#include <stdio.h>
#include <string.h>
#include "z_block_server.h"
#include "msg_define.h"
#include "common_utils.h"
#include <unistd.h>

//注册客户端
int register_login_client(int sockfd, const char *name, const char *password, int is_reg);
