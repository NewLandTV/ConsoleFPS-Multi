#include "Player.h"

Player::Player() : id(0), x(0.0f), y(0.0f), a(0.0f), clientSocket(0)
{

}

Player::Player(int id, float x, float y, float a, SOCKET clientSocket) : id(id), x(x), y(y), a(a), clientSocket(clientSocket)
{

}