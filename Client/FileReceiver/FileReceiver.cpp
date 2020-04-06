// FileReceiver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

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

	SOCKET client = accept(listener, NULL, NULL);
	char buf[2048];
	FILE* f = fopen("P:\\Code4Life\\NetworkProgramming\\Client\\FileReceiver\\receive.txt", "wb");
	int ret;
	int sum = 0;
	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);

		if (ret <= 0) {
			printf_s("Received %d bytes\n", sum);
			break;
		}
		fwrite(buf, 1, ret, f);
		sum += ret;
	}
	fclose(f);
	closesocket(client);
	WSACleanup();
}