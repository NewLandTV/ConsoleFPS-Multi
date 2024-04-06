#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <iostream>
#include <string>
#include <chrono>
#include <WinSock2.h>

// 콘솔 가로 세로
#define CONSOLE_WIDTH 120
#define CONSOLE_HEIGHT 40

// 맵 가로 세로
#define MAP_WIDTH 16
#define MAP_HEIGHT 16

#define BUFFER_SIZE 100

#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable:4996)

class Client
{
private:
	int id;

	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddrIn;

	char message[BUFFER_SIZE];

	int strLength;

	// 플레이어 정보
	float playerX = 8.0f;
	float playerY = 8.0f;
	float playerA = 0.0f;

	// Field of View와 Depth
	float FOV = 3.14159f / 4.0f;
	float depth = 16.0f;

	wchar_t* screen;
	HANDLE hConsole;
	DWORD dwBytesWritten;

	std::chrono::system_clock::time_point tp1;
	std::chrono::system_clock::time_point tp2;

public:
	void Init();
	void ConnectToServer(unsigned short port);
	void DisconnectToServer();
};

#endif