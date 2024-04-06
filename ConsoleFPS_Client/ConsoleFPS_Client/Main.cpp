/*
 * Project Name : Console FPS Client
 * Project Version : 0.1
 * Made By JkhTV
 */

#include "Client.h"

Client client;

bool CtrlHandler(DWORD fdwCtrlType);

int main(void)
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, true);

	client.Init();
	client.ConnectToServer(3877);
	client.DisconnectToServer();

	return 0;
}

bool CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	default:
		client.DisconnectToServer();

		return false;
	}
}