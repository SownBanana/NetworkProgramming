// OverlappedCompletionRoutin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")

typedef struct {
	OVERLAPPED overlapped;
	SOCKET client;
	char buf[256];
	WSABUF databuf;
} CLIENT_INFO;

void CALLBACK CompletionRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

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

	CLIENT_INFO clientInfo;

	clientInfo.client = accept(listener, NULL, NULL);

	memset(&clientInfo.overlapped, 0, sizeof(clientInfo.overlapped));
	clientInfo.databuf.buf = clientInfo.buf;
	clientInfo.databuf.len = sizeof(clientInfo.buf);

	DWORD byteReceived;
	DWORD flags = 0;
	int ret = WSARecv(clientInfo.client, &clientInfo.databuf, 1, &byteReceived, &flags, &clientInfo.overlapped, CompletionRoutine);

	while (1)
	{
		ret = SleepEx(WSA_INFINITE, TRUE);
		if (ret == 0)
		{
			printf("Timed Out.\n");
		}
	}
}

void CALLBACK CompletionRoutine(DWORD dwError, DWORD dwByteReceived, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	CLIENT_INFO* pClient = (CLIENT_INFO*)lpOverlapped;

	if (dwError != 0 || dwByteReceived == 0)
	{
		closesocket(pClient->client);
		return;
	}

	pClient->buf[dwByteReceived] = 0;
	printf("Received: %s\n", pClient->buf);

	DWORD flags = 0;
	DWORD byteReceived;
	WSARecv(pClient->client, &pClient->databuf, 1, &byteReceived, &flags, lpOverlapped, CompletionRoutine);
}