#include "z_send_file_list.h"

//创建节点
int f_create_node(Sending_File_Link *newnode)
{
    *newnode = (Sending_File_Link)malloc(sizeof(Sending_File_Node));

    if (*newnode == NULL)
    {
        printf("malloc error\n");
        return -1;
    }

    return 0;
}

//头插法插入
void f_insert_list_head(Sending_File_Link *head, Sending_File_Link newnode)
{
    if (*head == NULL)
    {
        *head = newnode;
        newnode->next = NULL;
    }
    else
    {
        newnode->next = (*head);
        (*head) = newnode;
    }
}

//删除节点
int f_delete_node(Sending_File_Link * send_list_head, unsigned char * recv_user_name,unsigned char * send_user_name)
{
    if (*send_list_head == NULL)
    {
        printf("sending file  list is empty\n");
        return -1;
    }

    Sending_File_Link p;
    Sending_File_Link q;
    p = *send_list_head;
    q = (*send_list_head)->next;


    if (recv_user_name == p->recv_user_name && send_user_name == p->send_user_name)
    {
        printf("delete head\n");
        free(p);
        p = NULL;
        *send_list_head = q;
        return 0;
    }

    while (q != NULL && (q->recv_user_name != recv_user_name || q->send_user_name != send_user_name))
    {
        p = q;
        q = q->next;
    }

    if(p->next != NULL)
    {
        p->next = q->next;
        free(q);
        q = NULL;
    }
    

    return 0;
}

//遍历链表
/*void display_list(Sending_File_Link head)
{
}*/

//销毁链表
void file_list_destory(Sending_File_Link head)
{
    Sending_File_Link p = head;
    if (head == NULL)
        return;

    while (p != NULL)
    {
        free(p);
        p = NULL;
        p = p->next;
    }
}