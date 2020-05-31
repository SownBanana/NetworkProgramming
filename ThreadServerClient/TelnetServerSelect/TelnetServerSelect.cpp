// TelnetServerSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
SOCKET tmpClients[40];
int numberOfClients;
int tmpNumClients;
FILE* fpout;
FILE* fpdata;

void removeClient(SOCKET);
SOCKET removeTmpClient(SOCKET);
bool checkLogin(char*, char*, FILE*);

int main()
{
	fpout = fopen("C:\\temp\\out.txt", "r");
	fpdata = fopen("C:\\temp\\data.txt", "r");
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
	tmpNumClients = 0;
	fd_set fdread;
	int countEvent;
	int ret;
	char buf[2048];
	const char getLogMss[256] = "Enter your Username and Password (user_name pass_word): ";
	const char wrongMss[256] = "Can't find your Username or Password\nReEnter (user_name pass_word):";
	const char succMss[256] = "Login Successful!\nStart your command\n>";
	const char* cmd = ">";

	char username[50];
	char password[50];

	while (true)
	{
		countEvent = 0;
		//Lam trong fdread
		FD_ZERO(&fdread);
		//Them cac socket vao fdread
		FD_SET(listener, &fdread);
		for (int i = 0; i < numberOfClients; i++) {
			FD_SET(clients[i], &fdread);
		}
		for (int i = 0; i < tmpNumClients; i++) {
			FD_SET(tmpClients[i], &fdread);
		}
		//Doi event
		ret = select(0, &fdread, 0, 0, 0);

		if (ret < 0) break;

		//Ket noi moi
		if (FD_ISSET(listener, &fdread)) {
			countEvent++;
			SOCKET client = accept(listener, NULL, NULL);
			printf("Client arrive\n");
			send(client, getLogMss, strlen(getLogMss), 0);

			tmpClients[tmpNumClients++] = client;
		}
		if (countEvent >= ret) continue;
		//Command tu client
		for (int i = 0; i < numberOfClients; i++) {
			if (FD_ISSET(clients[i], &fdread)) {
				countEvent++;
				//Thực hiện command
				int retR = recv(clients[i], buf, sizeof(buf), 0);
				if (retR < 0) {
					removeClient(clients[i]);
					i = -1;
					if (countEvent >= ret) break;
					continue;
				}
				//Bỏ dấu \n
				if (buf[retR - 1] == '\n')
					buf[retR - 1] = 0;
				else if (retR < sizeof(buf)) buf[retR] = 0;

				strcat(buf, " > C:\\temp\\out.txt");

				system(buf);
				while (true) {
					retR = fread(buf, 1, sizeof(buf), fpout);
					if (retR <= 0) break;
					send(clients[i], buf, retR, 0);
				}
				//Daau nhac lenh
				send(clients[i], cmd, sizeof(cmd), 0);
				rewind(fpout);

				if (countEvent >= ret) break;
			}
		}
		//Dang nhap
		for (int i = 0; i < tmpNumClients; i++) {
			if (FD_ISSET(tmpClients[i], &fdread)) {
				countEvent++;
				//Thực hiện command
				int retR = recv(tmpClients[i], buf, sizeof(buf), 0);
				if (retR < 0) {
					closesocket(removeTmpClient(tmpClients[i]));
					i = -1;
					if (countEvent >= ret) break;
					continue;
				}
				retR = sscanf(buf, "%s %s", username, password);
				if (checkLogin(username, password, fpdata)) {
					//chap nhan dang nhap
					send(tmpClients[i], succMss, sizeof(succMss), 0);
					clients[numberOfClients++] = tmpClients[i];
					removeTmpClient(tmpClients[i]);
				}
				else {
					send(tmpClients[i], wrongMss, sizeof(wrongMss), 0);
				}
				if (countEvent >= ret) break;
			}
		}
	}
	fclose(fpdata);
	fclose(fpout);
	closesocket(listener);
	WSACleanup();
}

bool checkLogin(char* inputUsername, char* inputPass, FILE* fpdata) {
	char username[50];
	char password[50];
	char aLine[256];
	//đọc từng dòng trong data.txt
	while (fgets(aLine, 100, fpdata) != NULL) {
		//Lấy username và pass
		sscanf(aLine, "%s %s", username, password);
		//Kiểm tra có khớp tên và pass nhập
		if (strcmp(username, inputUsername) != 0 || strcmp(password, inputPass) != 0) continue;

		rewind(fpdata);
		return true;
	}
	rewind(fpdata);

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
	closesocket(client);
}

SOCKET removeTmpClient(SOCKET client) {
	int i;
	for (i = 0; i < tmpNumClients; i++) {
		if (tmpClients[i] == client) break;
	}
	if (i == (tmpNumClients - 1)) tmpNumClients -= 1;
	else {
		tmpClients[i] = tmpClients[tmpNumClients - 1];
		tmpNumClients -= 1;
	}
	return client;
}