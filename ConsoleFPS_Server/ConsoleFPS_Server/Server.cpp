#include <thread>
#include "Server.h"

void Server::CompressPlayers(Player* playerArray, int omitIndex, int total)
{
	for (int i = omitIndex; i < total; i++)
	{
		playerArray[i] = playerArray[i + 1];
	}
}

void Server::CompressEvents(WSAEVENT* eventArray, int omitIndex, int total)
{
	for (int i = omitIndex; i < total; i++)
	{
		eventArray[i] = eventArray[i + 1];
	}
}

void Server::Log(const char* msg)
{
	std::cout << "[Log] " << msg << std::endl;
}

Server::Server()
{
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
}

void Server::Start(unsigned short port)
{
	Log("Server starting...");

	// WinSock �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		Log("WSAStartup() Error!");

		return;
	}

	// ���� ���� �ʱ�ȭ
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket == INVALID_SOCKET)
	{
		Log("socket() Error!");

		return;
	}

	// ���� ���ε�
	serverAddrIn.sin_family = AF_INET;
	serverAddrIn.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddrIn.sin_port = htons(port);

	if (bind(serverSocket, (sockaddr*)&serverAddrIn, sizeof(serverAddrIn)) == SOCKET_ERROR)
	{
		Log("bind() Error!");

		return;
	}

	// �̺�Ʈ �߻� Ȯ��
	newEvent = WSACreateEvent();

	if (WSAEventSelect(serverSocket, newEvent, FD_ACCEPT) == SOCKET_ERROR)
	{
		Log("WSAEventSelect() Error!");

		return;
	}

	// ���� ���
	if (listen(serverSocket, 5) == SOCKET_ERROR)
	{
		Log("listen() Error!");

		return;
	}

	playerArray[socketTotal] = Player(-1, 0.0f, 0.0f, 0.0f, serverSocket);

	eventArray[socketTotal] = newEvent;

	socketTotal++;

	Log("Started server!");

	while (1)
	{
		// �̺�Ʈ ���� ����
		int index = WSAWaitForMultipleEvents(socketTotal, eventArray, false, WSA_INFINITE, false);

		index -= WSA_WAIT_EVENT_0;

		for (int i = index; i < socketTotal; i++)
		{
			index = WSAWaitForMultipleEvents(1, &eventArray[i], true, 0, false);

			if (index == WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT)
			{
				continue;
			}

			index = i;

			WSAEnumNetworkEvents(*playerArray[index].GetClientSocket(), eventArray[index], &networkEvents);

			// �ʱ� ���� ��û ó��
			if (networkEvents.lNetworkEvents & FD_ACCEPT)
			{
				if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				{
					Log("Accept Error!");

					break;
				}

				clientLength = sizeof(clientAddrIn);

				// ���� ����
				clientSocket = accept(*playerArray[index].GetClientSocket(), (sockaddr*)&clientAddrIn, &clientLength);

				// �̺�Ʈ Ŀ�� ������Ʈ ����
				newEvent = WSACreateEvent();

				// �̺�Ʈ �߻� ���� Ȯ��
				WSAEventSelect(clientSocket, newEvent, FD_READ | FD_CLOSE);

				eventArray[socketTotal] = newEvent;

				playerArray[socketTotal] = Player(socketTotal, 8.0f, 8.0f, 0.0f, clientSocket);

				strcat_s(message, sizeof(message), std::to_string(playerArray[socketTotal].GetId()).c_str());

				send(clientSocket, message, strlen(message), 0);

				socketTotal++;

				// ���� ���� �α�
				Log("Client has connected to the server.");
				Log(std::to_string(playerArray[socketTotal - 1].GetId()).c_str());
			}

			// ������ ���� ó��
			if (networkEvents.lNetworkEvents & FD_READ)
			{
				if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
				{
					Log("Read Error!");

					break;
				}

				// TODO ���� �۾�

				int idx = index - WSA_WAIT_EVENT_0;

				// ������ �ޱ�
				strLength = recv(*playerArray[idx].GetClientSocket(), message, BUFFER_SIZE - 1, 0);

				// ������ ���� null �� �ֱ�
				message[strLength] = 0;

				char changedPlayerStat[BUFFER_SIZE];
				char* next;
				char* context = nullptr;

				memset(changedPlayerStat, 0, sizeof(changedPlayerStat));

				next = strtok_s(message, "|", &context);

				int cmd = std::stoi(next);

				next = strtok_s(nullptr, "|", &context);

				int cid = std::stoi(next);

				next = strtok_s(nullptr, "|", &context);

				float elapsedTime = std::stof(next);

				switch (cmd)
				{
					// ������ �̵� (W)
				case 0:
					playerArray[idx].SetX(playerArray[idx].GetX() + sinf(playerArray[idx].GetA()) * 5.0f * elapsedTime);
					playerArray[idx].SetY(playerArray[idx].GetY() + cosf(playerArray[idx].GetA()) * 5.0f * elapsedTime);

					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetX()).c_str());
					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), "|");
					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetY()).c_str());

					if (map[(int)playerArray[idx].GetY() * MAP_WIDTH + (int)playerArray[idx].GetX()] == '#')
					{
						memset(&changedPlayerStat, 0, sizeof(changedPlayerStat));

						playerArray[idx].SetX(playerArray[idx].GetX() - sinf(playerArray[idx].GetA()) * 5.0f * elapsedTime);
						playerArray[idx].SetY(playerArray[idx].GetY() - cosf(playerArray[idx].GetA()) * 5.0f * elapsedTime);

						strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetX()).c_str());
						strcat_s(changedPlayerStat, sizeof(changedPlayerStat), "|");
						strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetY()).c_str());
					}

					break;

					// �ڷ� �̵� (S)
				case 1:
					playerArray[idx].SetX(playerArray[idx].GetX() - sinf(playerArray[idx].GetA()) * 5.0f * elapsedTime);
					playerArray[idx].SetY(playerArray[idx].GetY() - cosf(playerArray[idx].GetA()) * 5.0f * elapsedTime);

					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetX()).c_str());
					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), "|");
					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetY()).c_str());

					if (map[(int)playerArray[idx].GetY() * MAP_WIDTH + (int)playerArray[idx].GetX()] == '#')
					{
						memset(&changedPlayerStat, 0, sizeof(changedPlayerStat));

						playerArray[idx].SetX(playerArray[idx].GetX() + sinf(playerArray[idx].GetA()) * 5.0f * elapsedTime);
						playerArray[idx].SetY(playerArray[idx].GetY() + cosf(playerArray[idx].GetA()) * 5.0f * elapsedTime);

						strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetX()).c_str());
						strcat_s(changedPlayerStat, sizeof(changedPlayerStat), "|");
						strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetY()).c_str());
					}

					break;

					// �������� ȸ�� (A)
				case 2:
					playerArray[idx].SetA(playerArray[idx].GetA() - 0.8f * elapsedTime);

					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetA()).c_str());

					break;

					// ���������� ȸ�� (D)
				case 3:
					playerArray[idx].SetA(playerArray[idx].GetA() + 0.8f * elapsedTime);

					strcat_s(changedPlayerStat, sizeof(changedPlayerStat), std::to_string(playerArray[idx].GetA()).c_str());

					break;
				}

				// �ٲ� �÷��̾� ���� ������
				send(*playerArray[idx].GetClientSocket(), changedPlayerStat, strlen(changedPlayerStat), 0);

				// ���� ������ ���
				Log(message);

				// �ٲ� �÷��̾� ���� ���
				Log(changedPlayerStat);
			}

			// ���� ���� ó��
			if (networkEvents.lNetworkEvents & FD_CLOSE)
			{
				if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
				{
					Log("Close Error!");

					break;
				}

				WSACloseEvent(eventArray[index]);

				// ���� ����
				closesocket(*playerArray[index].GetClientSocket());

				Log("Client has left the server.");

				socketTotal--;

				// �迭 ����
				CompressPlayers(playerArray, index, socketTotal);
				CompressEvents(eventArray, index, socketTotal);
			}
		}
	}

	// WinSock �Ҵ� ���� ���ҽ� ��ȯ
	WSACleanup();
}