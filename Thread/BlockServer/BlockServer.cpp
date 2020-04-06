#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	// Chap nhan ket noi va truyen nhan du lieu

	while (1) {
		printf("Waiting for new client\n");
		SOCKET client = accept(listener, NULL, NULL);
		//printf("New client accepted: %s\n", client);
		//send(client, "hi", sizeof("hi"), 0);
		char buf[256];
		int ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0) continue;
		//buf[ret] = 0;
		printf("Client: %s\n", buf);
		closesocket(client);
	}

	closesocket(listener);
	WSACleanup();
}