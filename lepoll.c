#include "lepoll.h"

int EpollAdd(int epoll_fd, int fd, void *user, int events) {
  struct epoll_event ev;
  ev.events = events | EPOLLERR;
  ev.data.ptr = user;
  epoll_ctl( epoll_fd,EPOLL_CTL_ADD,fd,&ev );
  return 0;
}

int EpollMod(int epoll_fd, int fd, void *user, int events) {
  struct epoll_event ev;
  ev.events = events | EPOLLERR;
  ev.data.ptr = user;
  //epoll_ctl( epoll_fd,EPOLL_CTL_ADD,fd,&ev ); 笔误导致未返回客户端
  epoll_ctl( epoll_fd,EPOLL_CTL_MOD,fd,&ev );
  return 0;
}

int EpollDel(int epoll_fd, int fd) {
  struct epoll_event ev;
  epoll_ctl( epoll_fd,EPOLL_CTL_DEL,fd,&ev );
  return 0;
}
