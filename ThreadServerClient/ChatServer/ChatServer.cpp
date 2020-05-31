// ChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
using namespace std;

#pragma comment(lib, "ws2_32")

DWORD WINAPI clientThread(LPVOID);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9090);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	// Chap nhan ket noi va truyen nhan du lieu
	printf("Waiting for a client\n");

	while (true)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client arrive\n");

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
	char buf[256];
	double a;
	double b;
	double rs;
	bool checkNumber = true; bool checkEr;
	char cmd[5], as[500], bs[500], tmp[19];
	while (true) {
		checkNumber = true; checkEr = false;
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret < 0) {
			closesocket(client);
			return 0;
		}
		//if(buf[ret - 1] == '/n')
		if (ret < sizeof(buf)) buf[ret] = '\0';
		printf("%s\n", buf);
		ret = sscanf(buf, "%s %s %s %s", cmd, as, bs, tmp);
		if (ret != 3) {
			sprintf(buf, "ERROR thua du lieu dau vao\n");
			send(client, buf, strlen(buf), 0);
		}
		else {
			for (int i = 0; i < strlen(as); i++) {
				if (!isdigit(as[i]) && as[i] != '.') {
					checkNumber = false;
					sprintf(buf, "ERROR A is not a number\n");
				}
			}
			for (int i = 0; i < strlen(bs); i++) {
				if (!isdigit(bs[i]) && bs[i] != '.') {
					checkNumber = false;
					sprintf(buf, "ERROR B is not a number\n");
				}
			}
			if (!checkNumber) {
				send(client, buf, strlen(buf), 0);
				continue;
			}
			a = atof(as);
			b = atof(bs);
			if (strcmp(cmd, "ADD") == 0) {
				rs = a + b;
			}
			else if (strcmp(cmd, "SUB") == 0) {
				rs = a - b;
			}
			else if (strcmp(cmd, "MUL") == 0) {
				rs = a * b;
			}
			else if (strcmp(cmd, "DIV") == 0) {
				if (b == 0.0) {
					checkEr = true;
					sprintf(buf, "ERROR divide for zero\n");
				}
				else {
					rs = a / b;
				}
			}
			else {
				sprintf(buf, "ERROR operator is not supported\n");
				checkEr = true;
			}
			if (!checkEr) sprintf(buf, "OK %f\n", rs);
			send(client, buf, strlen(buf), 0);
		}
	}
	return 0;
}