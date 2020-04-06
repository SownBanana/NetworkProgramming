// TelnetServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32")

SOCKET clients[40];
int numberOfClients;

DWORD WINAPI clientThread(LPVOID);
void removeClient(SOCKET);
char* checkUserName(char*, FILE*);

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

	char getUserMss[256] = "Enter your Username: ";
	char getPassMss[256] = "Enter your Password: ";
	char wrongUserMss[256] = "Can't find your Username\n";
	char wrongPassMss[256] = "Wrong Password\n ";
	char loginMss[256] = "Login Successful!\nStart your command\n";

	int ret;
	char buf[1024];
	FILE* fpout = fopen("C:\\temp\\out.txt", "r");
	FILE* fpdata = fopen("C:\\temp\\data.txt", "r");

	bool isLogined = false;
	while (!isLogined) {
		//Hỏi username
		send(client, getUserMss, strlen(getUserMss), 0);
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret < 0) {
			removeClient(client);
			closesocket(client);
			return 0;
		}
		if (ret < sizeof(buf)) buf[ret - 1] = '\0';

		//Kiểm tra có username không
		char* password = checkUserName(buf, fpdata);

		if (password != NULL) {
			char tmpPass[50];
			memcpy(tmpPass, password + 1, strlen(password));
			tmpPass[strlen(password)] = '\0';
			while (true) {
				send(client, getPassMss, strlen(getPassMss), 0);
				ret = recv(client, buf, sizeof(buf), 0);
				if (ret < 0) {
					removeClient(client);
					closesocket(client);
					return 0;
				}
				if (ret < sizeof(buf)) buf[ret] = '\0';
				if (strcmp(buf, tmpPass) == 0) {
					isLogined = true;
					break;
				}
				else send(client, wrongPassMss, strlen(wrongPassMss), 0);
			}
		}
		else send(client, wrongUserMss, strlen(wrongUserMss), 0);
	}
	send(client, loginMss, strlen(loginMss), 0);

	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret < 0) {
			break;
		}
		if (ret < sizeof(buf)) buf[ret - 1] = '\0';

		string a(buf);
		string command = a + " > C:\\temp\\out.txt";
		system(command.c_str());
		//Sleep(500);
		while (true) {
			ret = fread(buf, 1, sizeof(buf), fpout);
			if (ret <= 0) break;
			send(client, buf, ret, 0);
		}
		rewind(fpout);
	}
	removeClient(client);
	closesocket(client);
	return 0;
}

char* checkUserName(char* inputUsername, FILE* fpdata) {
	char username[50];
	char password[50];
	char aLine[256];
	while (fgets(aLine, 100, fpdata) != NULL) {
		sscanf(aLine, "%s", username);
		if (strcmp(username, inputUsername) != 0) continue;
		memcpy(password, aLine + strlen(username), 50);
		rewind(fpdata);
		return password;
	}
	rewind(fpdata);
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
		numberOfClients -= 1;
	}
}