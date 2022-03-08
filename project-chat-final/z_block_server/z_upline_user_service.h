#pragma once

#include <stdio.h>
#include <string.h>
#include "z_msg_define.h"
#include "z_client_list.h"
#include <sys/socket.h>
#include <stdlib.h>

int send_upline_users(int sockfd, int is_sendall);
