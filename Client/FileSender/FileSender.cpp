// FileSender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>

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
	FILE* f = fopen("P:\\Code4Life\\NetworkProgramming\\Client\\FileSender\\send.txt", "rb");
	char buf[2048];

	int sum = 0;
	while (true) {
		ret = fread(buf, 1, sizeof(buf), f);
		if (ret <= 0) {
			printf_s("Sended: %d bytes", sum);
			break;
		}
		send(client, buf, ret, 0);
		sum += ret;
	}
	fclose(f);
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