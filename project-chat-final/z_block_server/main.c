#include "z_block_server.h"
#include "z_chat_server_controller.h"
#include <signal.h>
void handle_pipe()
{
    //donothing
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("plese input port and ip\n");
        exit(0);
    }

    int port = atoi(argv[1]);
    char *ip = argv[2];
    printf("chat server start with %d %s\n", port, ip);

    handle_pie_main();
    //启动服务器
    int ret = z_start_server(port, ip);

    return ret;
}

//处理客户端pipe,防止客户端异常退出引发的pipe异常
void handle_pie_main()
{
    struct sigaction sa;
    sa.sa_handler=handle_pipe;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE,&sa,NULL);

}