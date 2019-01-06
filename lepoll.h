#ifndef LEPOLL_H_
#define LEPOLL_H_

#include <sys/epoll.h>

int EpollAdd(int epoll_fd, int fd, void *user, int events);
int EpollMod(int epoll_fd, int fd, void *user, int events);
int EpollDel(int epoll_fd, int fd);

#endif
