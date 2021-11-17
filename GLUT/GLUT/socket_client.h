#pragma once

#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_

#include <WinSock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

const int MAX_READ_BUF = 22800;

class Sock_client {
public:
	Sock_client();
	~Sock_client();

	bool Init(int port, char *address);

	bool Send_msg(std::string msg);
	bool Recv_msg(std::string &msg);

private:
	WSADATA wsaData;

	SOCKET sock;

	sockaddr_in server;
};

#endif//_SOCKET_CLIENT_H_
