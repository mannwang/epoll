#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "lepoll.h"
#include "tcp4.h"

#define MAX_EPOLL_EVENT 65536

static int g_listen_fd = -1; //监听描述符
static int g_epoll_fd = -1; //epoll描述符
static struct epoll_event *g_events = NULL;
static int max_conn = MAX_EPOLL_EVENT;

#define BUFSIZE 1024
struct ConnCache {
  int fd;
  char buf[BUFSIZE];
};

int main(int argc, char *argv[]) { //main ip port
  if (argc != 3) {
    printf("usage: %s IP PORT\n", argv[0]);
    return 0;
  }
  const char *ip = argv[1];
  short port = atoi(argv[2]);

  //初始化监听描述符
  g_listen_fd = CreateTcpSocket( port,ip,1 );
  listen( g_listen_fd,1024 );
  if (g_listen_fd == -1) {
    printf("Port %d is in use\n", port);
    return -1;
  }
  printf("listen [%d] %s:%d\n",g_listen_fd,ip,port);

  SetNonBlock( g_listen_fd );

  //初始化epoll描述符
  g_epoll_fd = epoll_create(max_conn);
  if (g_epoll_fd < 0) {
    printf("epoll_create error\n");
    return -1;
  }

  //监听描述符加入epoll中
  //EpollAdd( g_epoll_fd,g_listen_fd,(void*)g_listen_fd,EPOLLIN ); 把g_listen_fd值作为地址保存在data.ptr
  EpollAdd( g_epoll_fd,g_listen_fd,&g_listen_fd,EPOLLIN ); //把g_listen_fd地址保存在data.ptr

  //轮询查事件
  int i,num_event,ret,nbyte;
  void *ev_data = NULL;
  struct ConnCache *cc = NULL;
  g_events = (struct epoll_event*)malloc( max_conn*sizeof(struct epoll_event) );
  while (1) {
    num_event = epoll_wait( g_epoll_fd,g_events,max_conn,1); //可能被中断,返回继续执行
    for (i = 0; i < num_event; ++i) {
      ev_data = g_events[i].data.ptr;

      if (ev_data == &g_listen_fd) { //接受连接
        cc = (struct ConnCache*)malloc( sizeof(struct ConnCache) );
        cc->fd = accept( g_listen_fd,NULL,NULL );
        EpollAdd( g_epoll_fd,cc->fd,(void*)cc,EPOLLIN );
        printf("----------accept[%d]----------\n", cc->fd);
      } else { //处理连接
        struct ConnCache *cc = (struct ConnCache*)ev_data;
        if (g_events[i].events & EPOLLOUT) { //有数据写
          const char *fmt =
            "HTTP/1.1 200 OK\r\n\r\n"
            "<html>\n"
            "\t<h1>\n"
            "\t\tHello Epoll!\n"
            "\t</h1>\n"
            "</html>\n";
          //printf("----------response----------\n%s-------------------------\n", fmt);
          write( cc->fd,fmt,strlen(fmt) );
          //write( cc->fd,cc->buf,strlen(cc->buf) );
          //printf("----------response----------\n%s-------------------------\n", cc->buf);
          close( cc->fd );
          EpollDel( g_epoll_fd,cc->fd );
          free(cc);
        }
        if (g_events[i].events & EPOLLIN) { //有数据读
          {
            nbyte = read( cc->fd,cc->buf,sizeof(cc->buf) );
            ret = nbyte < 0 ? -errno: nbyte;
          }
          if(ret >= 0) {
            cc->buf[ret] = '\0';
            printf("----------request----------\n%s------------------------\n", cc->buf);
            EpollMod( g_epoll_fd,cc->fd,cc,EPOLLOUT ); //下次输出
          } else { //-EAGAIN时应该继续等待,暂时简化处理
            printf("client quit\n");
            close(cc->fd);
            EpollDel( g_epoll_fd,cc->fd );
            free(cc);
          }
        }
        if (g_events[i].events & EPOLLERR) {
          printf("Epoll Error!\n");
          close(cc->fd);
          EpollDel( g_epoll_fd,cc->fd );
          free(cc);
        }
      }
    }
  }

  return 0;
}
