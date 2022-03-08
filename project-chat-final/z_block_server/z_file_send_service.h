#pragma once
#include "z_client_list.h"
#include "z_msg_define.h"
#include <string.h>
#include <stdio.h>
#include "z_common_utils.h"
#include <sys/socket.h>
#include "z_send_file_list.h"

int file_send_request_service(int sockfd, const char *content);

int file_send_replay(int sockfd, const char *content);

int file_send_service(int sockfd, const char *content);