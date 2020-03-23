// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

	SOCKET client = accept(listener, NULL, NULL);
	char buf[256];

	int ret = recv(client, buf, sizeof(buf), 0);
	if (ret <= 0)
	{
		return 1;
	}
	
	buf[ret] = 0;
	printf_s("Client: %s\n", buf);

	while (true) {
		printf_s("Server: ");
		gets_s(buf);
		send(client, buf, sizeof(buf), 0);
		/*ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0) {
			printf_s("Connection lost\n");
			break;
		}

		if (ret < sizeof(buf)) buf[ret] = 0;
		printf_s("Client: %s\n", buf);*/
	}
	closesocket(client);
	WSACleanup();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
