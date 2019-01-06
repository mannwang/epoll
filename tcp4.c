#include "tcp4.h"
#include <string.h>
#include <fcntl.h>

void SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in *addr) {
  bzero(addr,sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(shPort);
  int nIP = 0;
  if(!pszIP
      || '\0' == *pszIP
      || 0 == strcmp(pszIP,"0")
      || 0 == strcmp(pszIP,"0.0.0.0")
      || 0 == strcmp(pszIP,"*")) {
    nIP = htonl(INADDR_ANY);
  } else {
    nIP = inet_addr(pszIP);
  }
  addr->sin_addr.s_addr = nIP;
}

int CreateTcpSocket(const unsigned short shPort, const char *pszIP, int bReuse) {
  int fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(fd >= 0) {
    if(shPort != 0) {
      if(bReuse) {
        int nReuseAddr = 1;
        setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&nReuseAddr,sizeof(nReuseAddr));
      }
      struct sockaddr_in addr;
      SetAddr( pszIP,shPort,&addr );
      int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
      if(ret != 0) {
        close(fd);
        return -1;
      }
    }
  }
  return fd;
}

int SetNonBlock(int fd) {
  int flags;
  flags = fcntl( fd,F_GETFL,0 );
  flags |= O_NONBLOCK;
  flags |= O_NDELAY;
  return fcntl( fd,F_SETFL,flags );
}

