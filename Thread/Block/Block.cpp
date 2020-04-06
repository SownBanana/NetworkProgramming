#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#pragma comment(lib, "ws2_32")

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	system("pause");
	int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));

	char buf[256];

	while (1) {
		gets_s(buf);
		send(client, buf, sizeof(buf), 0);

		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0) break;

		buf[ret] = 0;
		printf_s("Server: %s\n", buf);
	}

	closesocket(client);
	WSACleanup();
}