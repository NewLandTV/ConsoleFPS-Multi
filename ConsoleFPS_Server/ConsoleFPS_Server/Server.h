#ifndef __SERVER_H__
#define __SERVER_H__

#include <iostream>
#include <string>
#include <WinSock2.h>
#include "Player.h"

// �� ���� ����
#define MAP_WIDTH 16
#define MAP_HEIGHT 16

#define BUFFER_SIZE 100

#pragma comment(lib, "ws2_32.lib")

class Server
{
private:
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN serverAddrIn;

	// ���� ��û�� ���� ������ �����Ǵ� �÷��̾ �����ϴ� ����
	Player playerArray[WSA_MAXIMUM_WAIT_EVENTS];

	SOCKET clientSocket;
	SOCKADDR_IN clientAddrIn;

	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT newEvent;
	WSANETWORKEVENTS networkEvents;

	int clientLength;
	int socketTotal = 0;

	char message[BUFFER_SIZE];

	int strLength;

	std::wstring map;

	void CompressPlayers(Player* playerArray, int omitIndex, int total);
	void CompressEvents(WSAEVENT* eventArray, int omitIndex, int total);

protected:
	void Log(const char* msg);

public:
	Server();

	void Start(unsigned short port);
};

#endif