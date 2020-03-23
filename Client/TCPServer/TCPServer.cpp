// TCPServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

using namespace std;
int main(int argc, char const* argv[])
{
	FILE* hello_file;		//File chứa câu chào
	ofstream store_file;	//File lưu dữ liệu từ client (em dùng c++ vì ghi bằng fwrite bị lỗi mã hoá gì đó toàn ra tiếng TQ)
	int port;
	//Kiểm tra tham số dòng lệnh
	if (argc == 4) {

		if ((hello_file = fopen(argv[2], "rb")) == NULL) {
			printf("ERROR!!! %s is not exist.\n", argv[2]);
			return 0;
		}
		//Ghi thêm
		store_file.open(argv[3], ios::app);

		port = atoi(argv[1]);
	}
	//Nếu không truyền/đúng tham số dòng lệnh thì nhập lại
	else {
		char path[128];
		printf_s("Enter path hello file: ");
		gets_s(path);
		if ((hello_file = fopen(path, "rb")) == NULL) {
			printf("ERROR!!! %s is not exist.", path);
			return 0;
		}
		printf_s("Enter path store file: ");
		gets_s(path);
		//Mở để ghi thêm
		store_file.open(path, ios::app);
		printf_s("Enter port: ");
		scanf_s("%d", &port);
	}
	//Khởi tạo winsock
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf_s("ERROR");
	}
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Khởi tạo ip port
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	//Gán cổng
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	//Đợi client
	listen(listener, 5);
	printf("Waitting for Client...\n");
	SOCKET client = accept(listener, NULL, NULL);
	char buf[512];
	int ret;
	//Gửi câu chào
	while (true) {
		ret = fread(buf, 1, sizeof(buf), hello_file);
		if (ret <= 0) break;
		send(client, buf, ret, 0);
	}
	//Ghi dữ liệu từ client
	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		printf_s("%s\n", buf);
		if (ret <= 0) {
			break;
		}	
		store_file << buf << endl;
	}
	fclose(hello_file);
	store_file.close();
	closesocket(client);
	WSACleanup();
	return 0;
}
