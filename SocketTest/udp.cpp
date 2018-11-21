#include "stdafx.h"
#include "udp.h"
#include <iostream>
#include <WS2tcpip.h>

using namespace std;


udp::udp()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	cout << "Client Socketinitialized" << endl;
}

u_long nonblocking = 1;
long lngSendTimeOut = 1000;
void udp::setSockAddr(const char *destAddr, const int destPort)

{	//����Ϊ������ģʽ
	ioctlsocket(clientSocket, FIONBIO, &nonblocking);//������ģʽ��udp���Ͳ�ռʱ�䣬��ip�޹�
	setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&lngSendTimeOut, sizeof(lngSendTimeOut));
	setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&lngSendTimeOut, sizeof(lngSendTimeOut));

	clientSockAddr.sin_family = AF_INET;
	clientSockAddr.sin_port = htons(destPort);
	clientSockAddr.sin_addr.S_un.S_addr = inet_addr(destAddr);
	//inet_pton(AF_INET, destAddr, &clientSockAddr.sin_addr);
}

void udp::sendData(const char *buff, const int len)const {
	sendto(clientSocket, buff, len, 0, (SOCKADDR*)&clientSockAddr, sizeof(clientSockAddr));
}

void udp::release() {
	closesocket(clientSocket);
	WSACleanup();
}

udp::~udp()
{
}
