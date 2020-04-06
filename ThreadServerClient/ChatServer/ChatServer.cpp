// ChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32")

SOCKET clients[40];
int numberOfClients;

DWORD WINAPI clientThread(LPVOID);
bool checkValidNameSyntax(char*);
void broadcastMess(SOCKET, const char*);
void removeClient(SOCKET);

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
	printf("Waiting for a client\n");
	numberOfClients = 0;
	while (true)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client arrive\n");

		clients[numberOfClients++] = client;

		CreateThread(0, 0, clientThread, &client, 0, 0);
	}

	closesocket(listener);
	WSACleanup();
}

//Xử lý client
DWORD WINAPI clientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;
	int ret;
	char first[256] = "Enter your name (client_id: YOUR_NAME_HERE): ";
	char buf[256];
	send(client, first, sizeof(first), 0);
	ret = recv(client, buf, sizeof(buf), 0);
	if (ret < 0) return 0;
	if (ret < sizeof(buf)) buf[ret] = '\0';

	char invalidMess[256] = "Your syntax invalid! ReEnter your name in right syntax (client_id: YOUR_NAME_HERE): ";
	//Kiểm tra cú pháp
	while (!checkValidNameSyntax(buf)) {
		send(client, invalidMess, sizeof(invalidMess), 0);
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret < 0) {
			removeClient(client);
			closesocket(client);
			return 0;
		}
		if (ret < sizeof(buf)) buf[ret] = '\0';
	}

	//Đúng cú pháp - tách tên
	char clientName[128];
	memcpy(clientName, buf + 11, ret - 12);
	clientName[ret - 12] = '\0';
	//Gửi lời chào đến client
	//Lưu tên tạm
	string name(clientName, strlen(clientName));

	string wcmess;
	wcmess = "Welcome " + name + "\n";
	char* charWcMess = (char*)wcmess.c_str();
	send(client, charWcMess, strlen(charWcMess), 0);

	//Gửi thông báo đến các client khác
	string notiMess;
	notiMess = name + " entered the chat room.\n";
	char* charNotiMess = (char*)notiMess.c_str();
	broadcastMess(client, charNotiMess);

	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret < 0) {
			break;
		}
		if (ret < sizeof(buf)) buf[ret] = '\0';
		//printf_s("%s", buf);
		string strbuf(buf);
		string mess = name + ": " + strbuf;
		broadcastMess(client, mess.c_str());
	}
	string leaveMess = name + " leave the room\n";
	broadcastMess(client, leaveMess.c_str());
	removeClient(client);
	closesocket(client);
	return 0;
}

bool checkValidNameSyntax(char* name) {
	const char* p = name;
	const char* a = "client_id: ";
	int len = strlen(a);
	while (*p != NULL)
	{
		if (strlen(p) >= len)
		{
			if (strncmp(p, a, strlen(a)) == 0)
			{
				return true;
			}
		}
		else
		{
			break;
		}
		p++;
	}
	return false;
}

void removeClient(SOCKET client) {
	int i;
	for (i = 0; i < numberOfClients; i++) {
		if (clients[i] == client) break;
	}
	if (i == (numberOfClients - 1)) numberOfClients -= 1;
	else {
		clients[i] = clients[numberOfClients - 1];
		numberOfClients -= 1;
	}
}

void broadcastMess(SOCKET dst, const char* mess) {
	for (int i = 0; i <= numberOfClients; i++) {
		if (clients[i] != dst)
			send(clients[i], mess, strlen(mess), 0);
	}
}