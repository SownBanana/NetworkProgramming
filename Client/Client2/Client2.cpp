// Client2.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	system("pause");
	SOCKADDR_IN server_addr;
	addrinfo* info;

	int ret = getaddrinfo("bunoc.net", "http", NULL, &info);
	if (ret != 0) {
		printf_s("Ko phan giai duoc");
		return 1;
	}

	memcpy(&server_addr, info->ai_addr, info->ai_addrlen);

	ret = connect(client, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf_s("connect() failed: error code = %d", ret);
		return 1;
	}

	/*const char* mssg = "GET / HTTP/1.1\r\nHost: bunoc.net\r\n\r\n";*/
	const char* mssg = "HEAD /test.jpg HTTP/1.1\r\nHost: bunoc.net\r\n\r\n";
	send(client, mssg, strlen(mssg), 0);

	char buf[2048];

	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0) {
			printf_s("Connection lost\n");
			break;
		}

		if (ret < sizeof(buf)) buf[ret] = 0;
		printf_s("Receive: %s\n", buf);

		char* pos1 = strstr(buf, "Content-Length: ");
		if (pos1 != NULL) {
			char* pos2 = strstr(pos1, "\r\n");
			pos1[pos2 - pos1] = 0;
			//printf_s("%s\n", pos1);
			int filesize = atoi(pos1 + 16);
			printf_s("File size: %d bytes\n", filesize);
		}
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