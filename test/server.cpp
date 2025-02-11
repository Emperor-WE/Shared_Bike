#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>

#include "bike.pb.h"

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;    /* int == long */
typedef signed char     i8;
typedef signed short    i16;
typedef signed int      i32;    /* int == long */
typedef float           r32;
typedef double          r64;
typedef long double     r128;

using namespace std;
using namespace tutorial;

int tcp_connect_server(const char* server_ip, int port);

void cmd_msg_cb(int fd, short events, void* arg);
void server_msg_cb(struct bufferevent* bev, void* arg);
//void event_cb(struct bufferevent* bev, short event, void* arg);

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("please input 2 parameter\n");
        return -1;
    }

    int sockfd = tcp_connect_server(argv[1], atoi(argv[2]));
    if(sockfd == -1)
    {
        printf("tcp_connect error\n");
        return -1;
    }

    printf("connect to server successfully\n");

    struct event_base* base = event_base_new();
    struct bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);

    struct event* ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, cmd_msg_cb, (void*)bev);
    event_add(ev_cmd, NULL);

    bufferevent_setcb(bev, server_msg_cb, NULL, NULL, (void*)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    event_base_dispatch(base);

    printf("finished \n");
    return 0;
}

void cmd_msg_cb(int fd, short events, void* arg)
{
    char msg[1024];
    string proto_msg;

    int ret = read(fd, msg, sizeof(msg));
    if(ret < 0)
    {
        perror("real fail\n");
        exit(1);
    }

    msg[ret] = '\0';

    struct bufferevent* bev = (struct bufferevent*)arg;

    mobile_request mr;
    mr.set_mobile("32432535");

    int len = mr.ByteSizeLong();
    memcpy(msg, "FBEB", 4);

    *(u16*)(msg + 4) = 1;
    *(i32*)(msg + 6) = len;

    mr.SerializeToArray(msg + 10, len);

    bufferevent_write(bev, msg, len + 10);
}

int tcp_connect_server(const char* server_ip, int port)
{
    int sockfd, status, save_errno;
    struct sockaddr_in server_addr;
 
    memset(&server_addr, 0, sizeof(server_addr) );
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = inet_aton(server_ip, &server_addr.sin_addr);
 
    if( status == 0 ) //the server_ip is not valid value
    {
        errno = EINVAL;
        return -1;
    }
 
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if( sockfd == -1 )
        return sockfd;
 
 
    status = connect(sockfd, (sockaddr *)&server_addr, sizeof(server_addr) );
 
    if( status == -1 )
    {
        save_errno = errno;
        close(sockfd);
        errno = save_errno; //the close may be error
        return -1;
    }
 
    //evutil_make_socket_nonblocking(sockfd);
 
    return sockfd;
}

void server_msg_cb(struct bufferevent* bev, void* arg)
{
    char msg[1024];

    size_t len = bufferevent_read(bev, msg, sizeof(msg));
    msg[len] = '\0';

    printf("recv %s from server, len:%ld\n", msg, len);

    if(strncmp(msg, "FBEB", 4) == 0)
    {
        mobile_response mr;
        u16 code = *(u16*)(msg + 4);
        i32 len = *(i32*)(msg + 6);
        mr.ParseFromArray(msg + 10, len);
        printf("mobile_response: code:%d, icode:%d, data:%s\n", mr.code(), mr.icode(), mr.data().c_str());
    }
}