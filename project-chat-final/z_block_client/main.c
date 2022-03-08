#include <stdio.h>
#include "z_chat_client_controller.h"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("plese input port and IP\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    const char *ip = argv[2];
    printf("client start with port:%d  IP:%s\n", port, ip);

    z_chat_client_controller(port, ip);
    return 0;
}
