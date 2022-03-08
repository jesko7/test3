#include "msg_controller.h"

void msg_handle(const char *content)
{
    struct msg_define msg_struct;
    memset(&msg_struct, 0, sizeof(msg_struct));
    memcpy(&msg_struct, content, sizeof(msg_struct));

    if (msg_struct.is_send_all == 1)
    {
        printf("Receive %s brodest u msg:%s\n", msg_struct.user_name, msg_struct.msg);
    }
    else
    {
        printf("Receive %s send you msg:%s\n", msg_struct.user_name, msg_struct.msg);
    }
}