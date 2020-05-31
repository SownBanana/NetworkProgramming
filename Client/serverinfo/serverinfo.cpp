// serverinfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

typedef struct client_info {
	TCHAR client_name[256];
	int number_of_disk;
	char disk_list[100];
	int disk_size[10];
	int disk_free[10];
} client_info;

int main(int argc, char const* argv[])
{
	int port;
	//Kiểm tra tham số dòng lệnh
	if (argc == 2) {
		port = atoi(argv[1]);
	}
	//Nếu không truyền/đúng tham số dòng lệnh thì nhập lại
	else {
		printf_s("Enter port: ");
		scanf_s("%d", &port);
	}
	//Khởi tạo winsock
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf_s("ERRORR");
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

	client_info clientInfo;
	char buf[sizeof(client_info)];
	int ret;
	//Ghi dữ liệu từ client
	int test = sizeof(clientInfo);
	ret = recv(client, buf, sizeof(client_info), 0);
	memcpy(&clientInfo, buf, sizeof(client_info));
	//In ra màn hình
	wcout << "\nClient name: " << clientInfo.client_name << endl;
	cout << "Number of Disks: " << clientInfo.number_of_disk << endl;
	cout << "Disk    Total Space    Free Space" << endl;
	for (int i = 0; i < clientInfo.number_of_disk; i++) {
		cout << endl << clientInfo.disk_list[i * 8] << clientInfo.disk_list[i * 8 + 2] << clientInfo.disk_list[i * 8 + 4] << "       " << clientInfo.disk_size[i] << " GB         " << clientInfo.disk_free[i] << " GB\n";
	}
	closesocket(client);
	WSACleanup();
	return 0;
}