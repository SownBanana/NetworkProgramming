// TCPClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <regex>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
using namespace std;
int main(int argc, char const* argv[])
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf_s("ERROR");
		return 0;
	}

	int port;
	char* input_addr;
	//Regex ip xxx.xxx.xxx
	regex ip("\\b\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\b", regex_constants::icase);
	//Regex domain name xx.xxx
	regex domain("[\\w-]*\\.[\\w.-]*", regex_constants::icase);
	bool ip_match = false;
	bool domain_match = false;
	//Kiểm tra tham số dòng lệnh
	if (argc == 3) {
		input_addr = (char*)argv[1];
		port = atoi(argv[2]);
	}
	//Nếu không truyền/đúng tham số dòng lệnh thì nhập lại
	else {
		input_addr = new char[256];
		printf_s("Enter the Domain/IPv4 address: ");
		cin.getline(input_addr, 256);
		printf_s("Enter port: ");
		scanf_s("%d", &port);
	}
	//Xử lý tên miền/ip
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	ip_match = regex_match(input_addr, ip);
	domain_match = regex_match(input_addr, domain);
	//Nếu là ip
	if (ip_match) {
		server_addr.sin_addr.s_addr = inet_addr(input_addr);
	}
	//Nếu là domain
	else if (domain_match) {
		//Phân giải tên miền
		addrinfo* info;         // Lay info
		int ret = getaddrinfo(input_addr, "http", NULL, &info);
		if (ret == 0) {
			memcpy(&server_addr, info->ai_addr, info->ai_addrlen);
			printf_s("ip: %s", inet_ntoa(server_addr.sin_addr));
		}
		else {
			printf("Cant find domain\n");
			return 0;
		}
	}
	//localhost
	else if (strcmp(input_addr, "localhost") == 0) {
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	//Sai
	else {
		printf("Domain/IPv4 address is not valid");
		return 0;
	}

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	int ret = connect(client, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf_s("connect() failed: error code = %d", ret);
		return 1;
	}

	char buf[512];
	//Nhận dữ liệu từ Server in ra màn hình
	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0) {
			break;
		}
		if (ret < sizeof(buf)) {
			buf[ret] = 0;
			puts(buf);
			break;
		}
		puts(buf);
	}
	//Gửi dữ liệu cho server từ bàn phím
	while (true) {
		gets_s(buf);
		//fgets(buf, sizeof(buf), stdin);
		if (strcmp(buf, "exit") == 0) break;
		send(client, buf, sizeof(buf), 0);
	}

	closesocket(client);
	WSACleanup();
}
