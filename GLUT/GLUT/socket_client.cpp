#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include "stdafx.h"
#include "socket_client.h"

Sock_client::Sock_client() {
	printf("start socket\n");
}

Sock_client::~Sock_client() {
	closesocket(sock);
	WSACleanup();
	Sleep(1000);
}

bool Sock_client::Init(int port, char *address) {
	int isStartUp = WSAStartup(MAKEWORD(2, 0), &wsaData);

	if (isStartUp != 0) {
		printf("[ERROR][sock_client : Init] start up failed\n");
		return false;
	}
	else {
		printf("[ O K ][sock_client : Init] start up success\n");
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = inet_addr(address);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET) {
		printf("[ERROR][sock_client : Init] socket failed\n");
		return false;
	}
	else {
		printf("[ O K ][sock_client : Init] socket created\n");
	}

	int isConnect = connect(sock, (struct sockaddr*)&server, sizeof(server));

	if (isConnect == SOCKET_ERROR) {
		printf("[ERROR][sock_client : Init] connect failed\n");
		return false;
	}
	else {
		printf("[ O K ][sock_client : Init] connect success\n");
	}

	return true;
}

bool Sock_client::Send_msg(std::string msg) {
	int isSend = send(sock, msg.c_str(), msg.size(), 0);

	if (isSend == SOCKET_ERROR) {
		printf("[ERROR][sock_client : send_msg] send message failed\n");
		return false;
	}

	return true;
}

bool Sock_client::Recv_msg(std::string &msg) {
	char RecBuf[MAX_READ_BUF];

	int isRecv = recv(sock, RecBuf, sizeof(RecBuf), 0);

	if (isRecv == SOCKET_ERROR) {
		printf("[ERROR][sock_client : recv_msg] receive message failed\n");
		return false;
	}

	msg = std::string(RecBuf);
	return true;
}