#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "tcp4.h"
#include "lutils.h"

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("usage: %s IP PORT\n", argv[0]);
    return 0;
  }
  const char *ip = argv[1];
  short port = atoi(argv[2]);

  int maxfd, stdineof = 0;
  fd_set rset;
  char buf[BUFSIZE];
  struct sockaddr_in addr;
  SetAddr( ip,port,&addr );
  int fd = CreateTcpSocket( 0,"0",0 );
  int ret = connect( fd,(struct sockaddr*)&addr,sizeof(addr) );
  if (ret < 0) {
    //fd非阻塞时,目标没有及时返回,返回EINPROGRESS=115,Operation now in progress
    printf("connect error %d - %s\n", errno, strerror(errno));
    return 0;
  }
  SetNonBlock( fd );

  FD_ZERO(&rset);

  int n;
  for ( ; ; ) {
    if (stdineof == 0) {
      FD_SET( fileno(stdin),&rset );
    }
    FD_SET( fd,&rset );
    maxfd = (max( fd,fileno(stdin) )) + 1;
    n = select( maxfd,&rset,NULL,NULL,NULL );

    if (FD_ISSET( fd,&rset )) { //可读
      if ((n=read( fd,buf,BUFSIZE )) == 0) { //没数据了
        if (stdineof == 1) { //正常结束
          return 0;
        } else {
          printf("server terminated prematurely\n");
          return 0;
        }
      }
      write( fileno(stdin),buf,n );
    }
    if (FD_ISSET( fileno(stdin),&rset )) { //输入可读
      if ((n=read( fileno(stdin),buf,BUFSIZE )) == 0) {
        stdineof = 1;
        shutdown( fd,SHUT_WR ); //发送FIN
        FD_CLR( fileno(stdin),&rset );
        continue;
      }
      write( fd,buf,n );
    }
  }
  return 0;
}
