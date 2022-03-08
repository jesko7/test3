#include "z_client_list.h"

int create_node(Link *newnode)
{
    *newnode = (Link)malloc(sizeof(Node));

    if (*newnode == NULL)
    {
        printf("malloc error\n");
        return -1;
    }

    return 0;
}

void insert_list_head(Link *head, Link newnode)
{
    if ((*head) == NULL)
    {
        (*head) = newnode;
        newnode->next = NULL;
    }
    else
    {
        newnode->next = (*head);
        (*head) = newnode;
    }
}

int delete_node(Link *head, int sockfd)
{
    if (*head == NULL)
    {
        printf("client list is empty\n");
        return -1;
    }

    Link p, q;
    p = q = (*head);

    if ((*head)->sockfd == sockfd)
    {
        printf("delete head\n");
        (*head) = (*head)->next;
        free(p);
        return 0;
    }

    while (p != NULL && p->sockfd != sockfd)
    {
        q = p;
        p = p->next;
    }

    /*sockfd肯定存在，移动到最后一个*/
    if (q->next != NULL)
    {
        q->next = p->next;
        free(p);
        p = NULL;
    }

    return 0;
}

void display_list(Link head)
{
    Link p = head;
    if (head == NULL)
    {
        printf("链表为空\n");
        return;
    }

    while (p != NULL)
    {
        printf("client ip:%s port:%s fd:%d\n", p->user_name, p->pass_word, p->sockfd);
        p = p->next;
    }
}

void destory(Link head)
{
    Link p = head;
    if (head == NULL)
        return;

    while (p != NULL)
    {
        free(p);
        p = NULL;
        p = p->next;
    }
}