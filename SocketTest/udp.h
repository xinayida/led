#pragma once

#ifndef UDP_H
#define UDP_H

#include <WinSock2.h>

class udp
{
private:
	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN clientSockAddr;
public:
	udp();
	void setSockAddr(const char *destAddr, const int destPort);
	void sendData(const char *buf, const int len) const;
	void release();
	~udp();
};

#endif