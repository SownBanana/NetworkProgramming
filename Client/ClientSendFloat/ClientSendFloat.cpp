// ClientSendFloat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
int main()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf_s("ERROR");
	}
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	system("pause");
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(9000);

	int ret = connect(client, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf_s("connect() failed: error code = %d", ret);
		return 1;
	}
	printf_s("Connect success\n");
	float f;
	while (true)
	{
		scanf_s("%f", &f);
		send(client, (char*)&f, sizeof(f), 0);
		if (f == 0) break;
	}
	
	closesocket(client);
	WSACleanup();
}
