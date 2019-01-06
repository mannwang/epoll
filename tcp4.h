#ifndef TCP4_H_
#define TCP4_H_

#include <sys/socket.h>
#include <netinet/in.h>

void SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in *addr);
int CreateTcpSocket(const unsigned short shPort, const char *pszIP, int bReuse);
int SetNonBlock(int iSock);

#endif
