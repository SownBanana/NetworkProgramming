// ChatServerSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
SOCKET clientsTmp[40];
char* ids[40];
int numberOfClients;
int numberOfClientsTmp;

char* checkValidNameSyntax(char*);
void broadcastMess(SOCKET, const char*);
void removeClient(SOCKET);
SOCKET removeClientTmp(SOCKET);

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

	const char getClientIdMess[256] = "Enter your name (client_id: YOUR_NAME_HERE): ";
	const char invalidMess[256] = "Your syntax invalid!\n ReEnter your name in right syntax (client_id: YOUR_NAME_HERE): ";

	numberOfClients = 0;

	fd_set fdread;
	int ret;
	char buf[256];
	char sendMess[300];
	int countEvent;

	while (true)
	{
		countEvent = 0;
		//xoá hết fdread
		FD_ZERO(&fdread);
		//thêm socket vào fdread
		FD_SET(listener, &fdread);
		for (int i = 0; i < numberOfClients; i++) {
			FD_SET(clients[i], &fdread);
		}
		for (int i = 0; i < numberOfClientsTmp; i++) {
			FD_SET(clientsTmp[i], &fdread);
		}
		//đợi sự kiên
		ret = select(0, &fdread, 0, 0, 0);

		//Kiểm tra
		if (ret < 0) break;

		if (FD_ISSET(listener, &fdread)) {
			countEvent++;
			SOCKET client = accept(listener, NULL, NULL);

			send(client, getClientIdMess, strlen(getClientIdMess), 0);
			clientsTmp[numberOfClientsTmp++] = client;
		}
		if (countEvent >= ret) continue;
		//Nhan Tin
		for (int i = 0; i < numberOfClients; i++) {
			if (FD_ISSET(clients[i], &fdread)) {
				countEvent++;
				printf("%s", ids[i]);
				int retR = recv(clients[i], buf, sizeof(buf), 0);
				if (retR < 0) {
					const char* leaveMess = "is leaving the room.\n";
					sprintf(sendMess, "%s %s", ids[i], leaveMess);
					broadcastMess(clients[i], sendMess);
					removeClient(clients[i]);
					i = -1;
					if (countEvent >= ret) break;
					continue;
				}
				if (retR < sizeof(buf)) buf[retR] = '\0';
				sprintf(sendMess, "%s: %s", ids[i], buf);
				broadcastMess(clients[i], sendMess);
				if (countEvent >= ret) break;
			}
		}
		//Nhap clientID
		for (int i = 0; i < numberOfClientsTmp; i++) {
			if (FD_ISSET(clientsTmp[i], &fdread)) {
				countEvent++;

				int retR = recv(clientsTmp[i], buf, sizeof(buf), 0);
				if (retR < 0) {
					closesocket(removeClientTmp(clientsTmp[i]));
					i = -1;
					if (countEvent >= ret) break;
					continue;
				}
				if (retR < sizeof(buf)) buf[retR] = 0;

				ids[numberOfClients] = new char[40];
				char* check = checkValidNameSyntax(buf);
				if (check) {
					memcpy(ids[numberOfClients], check, strlen(check));
					ids[numberOfClients][strlen(check)] = 0;

					//Chuyen SOCKET sang clients
					clients[numberOfClients++] = clientsTmp[i];
					removeClientTmp(clientsTmp[i]);
					printf("Client arrive\n");
					//Gửi lời chào đến client
					//Lưu tên tạm

					char wcmess[100];
					sprintf(wcmess, "Welcome %s to the Chatroom!\n", ids[numberOfClients - 1]);
					send(clients[numberOfClients - 1], wcmess, strlen(wcmess), 0);

					////Gửi thông báo đến các client khác
					sprintf(wcmess, "%s entered the Chatroom!", ids[numberOfClients - 1]);
					broadcastMess(clients[numberOfClients - 1], wcmess);
				}
				else {
					send(clientsTmp[i], invalidMess, sizeof(invalidMess), 0);
				}
				if (countEvent >= ret) break;
			}
		}
	}

	closesocket(listener);
	WSACleanup();
}

//Kiểm tra syntax
char* checkValidNameSyntax(char* buf) {
	char name[50];
	char syn[50];
	char tmp[50];
	const char* a = "client_id:";

	int ret = sscanf(buf, "%s %s", syn, name, tmp);
	if (ret != 2) return NULL;
	else if (strcmp(syn, a) == 0) return name;
	return NULL;
}

void removeClient(SOCKET client) {
	int i;
	for (i = 0; i < numberOfClients; i++) {
		if (clients[i] == client) break;
	}
	if (i == (numberOfClients - 1)) numberOfClients -= 1;
	else {
		clients[i] = clients[numberOfClients - 1];
		ids[i] = ids[numberOfClients - 1];
		numberOfClients -= 1;
	}
	closesocket(client);
}
SOCKET removeClientTmp(SOCKET client) {
	int i;
	for (i = 0; i < numberOfClientsTmp; i++) {
		if (clientsTmp[i] == client) break;
	}
	if (i == (numberOfClientsTmp - 1)) numberOfClientsTmp -= 1;
	else {
		clientsTmp[i] = clientsTmp[numberOfClientsTmp - 1];
		numberOfClientsTmp -= 1;
	}
	return client;
}

void broadcastMess(SOCKET dst, const char* mess) {
	for (int i = 0; i <= numberOfClients; i++) {
		if (clients[i] != dst)
			send(clients[i], mess, strlen(mess), 0);
	}
}