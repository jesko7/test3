#pragma once

#include <stdio.h>
#include <stdlib.h>

struct sending_file_info
{
    unsigned char send_user_name[32];
    unsigned char recv_user_name[32];
    int recv_fd;
    struct sending_file_info *next;
};

typedef struct sending_file_info Sending_File_Node;
typedef struct sending_file_info * Sending_File_Link;

//创建节点
int f_create_node(Sending_File_Link *newnode);

//头插法插入
void f_insert_list_head(Sending_File_Link *head, Sending_File_Link newnode);

//删除节点
int f_delete_node(Sending_File_Link * send_list_head, unsigned char * recv_user_name,unsigned char * send_user_name);

//遍历链表
// void display_list(Sending_File_Link head);

//销毁链表
void file_list_destory(Sending_File_Link head);