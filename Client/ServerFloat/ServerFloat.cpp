// ServerFloat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
int main()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf_s("ERROR");
	}
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);
	printf_s("wait for client...\n");
	SOCKET client = accept(listener, NULL, NULL);
	
	float f;
	int ret;
	while (true)
	{
		ret = recv(client, (char*)&f, sizeof(f), 0);
		if (f == 0.0) break;
		printf_s("%d bytes received: %f\n", ret, f);
	}

	closesocket(client);
	WSACleanup();
}