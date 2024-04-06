#include <vector>
#include <algorithm>
#include "Client.h"

std::wstring map;

void Client::Init()
{
	system("mode con cols=120 lines=40");

	// Create Screen Buffer
	screen = new wchar_t[CONSOLE_WIDTH * CONSOLE_HEIGHT];

	hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hConsole);

	dwBytesWritten = 0;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	tp1 = std::chrono::system_clock::now();
	tp2 = std::chrono::system_clock::now();
}

void Client::ConnectToServer(unsigned short port)
{
	std::cout << "Connecting to server..." << std::endl;

	// WinSock 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "WSAStartup() error!";

		return;
	}

	// 클라이언트 소켓 초기화
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "socket() Error!";

		return;
	}

	memset(&serverAddrIn, 0, sizeof(serverAddrIn));

	serverAddrIn.sin_family = AF_INET;
	serverAddrIn.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serverAddrIn.sin_port = htons(port);

	// 서버 접속
	if (connect(clientSocket, (sockaddr*)&serverAddrIn, sizeof(serverAddrIn)) == SOCKET_ERROR)
	{
		std::cout << "connect() Error!";

		return;
	}

	memset(&message, 0, sizeof(message));

	// 아이디 수신
	strLength = recv(clientSocket, message, BUFFER_SIZE - 1, 0);

	// 데이터 끝에 null 값 넣기
	message[strLength] = 0;

	id = std::stoi(message);

	while (1)
	{
		tp2 = std::chrono::system_clock::now();

		std::chrono::duration<float> elapsedTime = tp2 - tp1;

		tp1 = tp2;

		float floatElapsedTime = elapsedTime.count();

		char* next;
		char* context = nullptr;

		memset(&message, 0, sizeof(message));

		// Controls
		// Handle CCW Rotation
		if (GetAsyncKeyState(65) & 0x8000)
		{
			strcat_s(message, sizeof(message), "2|");
			strcat_s(message, sizeof(message), std::to_string(id).c_str());
			strcat_s(message, sizeof(message), "|");
			strcat_s(message, sizeof(message), std::to_string(floatElapsedTime).c_str());

			// 메시지 전송
			send(clientSocket, message, strlen(message), 0);

			// 메시지 수신
			strLength = recv(clientSocket, message, BUFFER_SIZE - 1, 0);

			// 데이터 끝에 null 값 넣기
			message[strLength] = 0;

			playerA = std::stof(message);

			memset(&message, 0, sizeof(message));
		}
		if (GetAsyncKeyState(68) & 0x8000)
		{
			strcat_s(message, sizeof(message), "3|");
			strcat_s(message, sizeof(message), std::to_string(id).c_str());
			strcat_s(message, sizeof(message), "|");
			strcat_s(message, sizeof(message), std::to_string(floatElapsedTime).c_str());

			// 메시지 전송
			send(clientSocket, message, strlen(message), 0);

			// 메시지 수신
			strLength = recv(clientSocket, message, BUFFER_SIZE - 1, 0);

			// 데이터 끝에 null 값 넣기
			message[strLength] = 0;

			playerA = std::stof(message);

			memset(&message, 0, sizeof(message));
		}
		if (GetAsyncKeyState(87) & 0x8000)
		{
			strcat_s(message, sizeof(message), "0|");
			strcat_s(message, sizeof(message), std::to_string(id).c_str());
			strcat_s(message, sizeof(message), "|");
			strcat_s(message, sizeof(message), std::to_string(floatElapsedTime).c_str());

			// 메시지 전송
			send(clientSocket, message, strlen(message), 0);

			// 메시지 수신
			strLength = recv(clientSocket, message, BUFFER_SIZE - 1, 0);

			// 데이터 끝에 null 값 넣기
			message[strLength] = 0;

			next = strtok_s(message, "|", &context);

			float x = std::stof(next);

			next = strtok_s(nullptr, "|", &context);

			float y = std::stof(next);

			playerX = x;
			playerY = y;

			memset(&message, 0, sizeof(message));
		}
		if (GetAsyncKeyState(83) & 0x8000)
		{
			strcat_s(message, sizeof(message), "1|");
			strcat_s(message, sizeof(message), std::to_string(id).c_str());
			strcat_s(message, sizeof(message), "|");
			strcat_s(message, sizeof(message), std::to_string(floatElapsedTime).c_str());

			// 메시지 전송
			send(clientSocket, message, strlen(message), 0);

			// 메시지 수신
			strLength = recv(clientSocket, message, BUFFER_SIZE - 1, 0);

			// 데이터 끝에 null 값 넣기
			message[strLength] = 0;

			next = strtok_s(message, "|", &context);

			float x = std::stof(next);

			next = strtok_s(nullptr, "|", &context);

			float y = std::stof(next);

			playerX = x;
			playerY = y;
		}

		memset(&message, 0, sizeof(message));

		for (int x = 0; x < CONSOLE_WIDTH; x++)
		{
			// For each column, calculate the projected ray angle into world space
			float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)CONSOLE_WIDTH) * FOV;

			float distanceToWall = 0.0f;
			bool hitWall = false;
			bool boundary = false;

			// Unit vector for ray in player space
			float eyeX = sinf(rayAngle);
			float eyeY = cosf(rayAngle);

			while (!hitWall && distanceToWall < depth)
			{
				distanceToWall += 0.1f;

				int testX = (int)(playerX + eyeX * distanceToWall);
				int testY = (int)(playerY + eyeY * distanceToWall);

				// Test if ray is out of bounds
				if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT)
				{
					// Just set distance to maximum depth
					hitWall = true;
					distanceToWall = depth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map[testY * MAP_WIDTH + testX] == '#')
					{
						hitWall = true;

						// distance, dot
						std::vector<std::pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
						{
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)testY + ty - playerY;
								float vx = (float)testX + tx - playerX;

								float d = sqrt(vx * vx + vy * vy);

								float dot = (eyeX * vx / d) + (eyeY * vy / d);

								p.push_back(std::make_pair(d, dot));
							}
						}

						// Sort Pairs from closest to farthest
						std::sort(p.begin(), p.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right) { return left.first < right.first; });

						float bound = 0.01f;

						if (acos(p.at(0).second) < bound)
						{
							boundary = true;
						}
						if (acos(p.at(1).second) < bound)
						{
							boundary = true;
						}
						/*if (acos(p.at(2).second) < bound)
						{
							boundary = true;
						}*/
					}
				}
			}

			// Calculate distance to ceiling and floor
			int ceiling = (float)(CONSOLE_HEIGHT / 2.0f) - CONSOLE_HEIGHT / ((float)distanceToWall);
			int floor = CONSOLE_HEIGHT - ceiling;

			short shade = ' ';

			if (distanceToWall <= depth / 4.0f)
			{
				// Very close
				shade = 0x2588;
			}
			else if (distanceToWall < depth / 3.0f)
			{
				shade = 0x2593;
			}
			else if (distanceToWall < depth / 2.0f)
			{
				shade = 0x2592;
			}
			else if (distanceToWall < depth)
			{
				shade = 0x2591;
			}
			else
			{
				// Too far away
				shade = ' ';
			}

			if (boundary)
			{
				// Black it out
				shade = ' ';
			}

			for (int y = 0; y < CONSOLE_HEIGHT; y++)
			{
				if (y <= ceiling)
				{
					screen[y * CONSOLE_WIDTH + x] = ' ';
				}
				else if (y > ceiling && y <= floor)
				{
					screen[y * CONSOLE_WIDTH + x] = shade;
				}
				else
				{
					// Shade floor based on distance
					float b = 1.0f - (((float)y - CONSOLE_HEIGHT / 2.0f) / ((float)CONSOLE_HEIGHT / 2.0f));

					if (b < 0.25f)
					{
						shade = '#';
					}
					else if (b < 0.5f)
					{
						shade = 'x';
					}
					else if (b < 0.75f)
					{
						shade = '.';
					}
					else if (b < 0.9f)
					{
						shade = '-';
					}
					else
					{
						shade = ' ';
					}

					screen[y * CONSOLE_WIDTH + x] = shade;
				}
			}
		}

		// Display Stats
		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", playerX, playerY, playerA, 1.0f / floatElapsedTime);

		// Display Map
		for (int nx = 0; nx < MAP_WIDTH; nx++)
		{
			for (int ny = 0; ny < MAP_HEIGHT; ny++)
			{
				screen[(ny + 1) * CONSOLE_WIDTH + nx] = map[ny * MAP_WIDTH + nx];
			}
		}

		screen[((int)playerY + 1) * CONSOLE_WIDTH + (int)playerX] = 'P';

		screen[CONSOLE_WIDTH * CONSOLE_HEIGHT - 1] = '\0';

		WriteConsoleOutputCharacter(hConsole, screen, CONSOLE_WIDTH * CONSOLE_HEIGHT, { 0, 0 }, &dwBytesWritten);
	}

	closesocket(clientSocket);

	// WinSock 할당 받은 리소스 반환
	WSACleanup();
}

void Client::DisconnectToServer()
{
	delete[] screen;

	closesocket(clientSocket);

	// WinSock 할당 받은 리소스 반환
	WSACleanup();
}