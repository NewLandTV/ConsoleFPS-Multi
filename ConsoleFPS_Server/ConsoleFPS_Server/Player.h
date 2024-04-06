#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <WinSock2.h>

class Player
{
private:
	int id;
	float x;
	float y;
	float a;

	SOCKET clientSocket;

public:
	Player();
	Player(int id, float x, float y, float a, SOCKET clientSocket);

	// Get
	int GetId() { return id; }
	float GetX() { return x; }
	float GetY() { return y; }
	float GetA() { return a; }

	SOCKET* GetClientSocket() { return &clientSocket; };

	// Set
	void SetId(int id) { this->id = id; }
	void SetX(float x) { this->x = x; }
	void SetY(float y) { this->y = y; }
	void SetA(float a) { this->a = a; }

	void SetClientSocket(SOCKET clientSocket) { this->clientSocket = clientSocket; };
};

#endif