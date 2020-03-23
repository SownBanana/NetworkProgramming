// clientinfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <regex>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "user32.lib")

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
	cout << "Connected to Server!";
	client_info clientInfo;

	DWORD bufSize;
	//Tên thiết bị
	wchar_t client_name[256];
	if (!GetComputerName(client_name, &bufSize)) printf("Error while getting desktop name");
	//Danh sách ổ đĩa
	bufSize = 100;
	char listDisk[100];
	int numberofDisk = GetLogicalDriveStrings(bufSize, (LPWSTR)listDisk);
	if (!numberofDisk) printf("Error while getting list disk");

	//Kích thước ổ đĩa
	int size = numberofDisk / 4; //real number of disk

	wchar_t disk[3];		//chứa tên ổ đĩa L"D:"
	__int64 avail, total, free;
	
	int* sizeofdisk = new int[size];  //Tổng dung lượng ổ đĩa theo thứ tự
	int* freeofdisk = new int[size];  //Dung lượng trống
	int index = 0;
	bool checkInsert;					//Kiểm tra insert đĩa chưa. VD: DVD
	for (int i = 0; i < numberofDisk * 2; i += 8) {
		disk[0] = listDisk[i];
		disk[1] = listDisk[i + 2];
		disk[2] = 0;
		checkInsert = GetDiskFreeSpaceEx(disk, (PULARGE_INTEGER)&avail, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
		if (checkInsert) {
			sizeofdisk[index] = total / (1024 * 1024 * 1024);  //GB
			freeofdisk[index++] = free / (1024 * 1024 * 1024);
		}
		else {
			sizeofdisk[index] = 0;
			freeofdisk[index++] = 0;
		}
	}


	//Chuyển dữ liệu vào struct
	memcpy(clientInfo.client_name, client_name, sizeof(client_name));
	clientInfo.number_of_disk = size;
	memcpy(clientInfo.disk_list, listDisk, sizeof(listDisk));
	for (int i = 0; i < size; i++) {
		clientInfo.disk_size[i] = sizeofdisk[i];
		clientInfo.disk_free[i] = freeofdisk[i];
	}
	
	//Gửi dữ liệu cho server
	cout << "\nSending information";
	char buf[sizeof(client_info)];
	memcpy(buf, &clientInfo, sizeof(client_info));
	int bytesSent = send(client, buf, sizeof(client_info), 0);
	if (bytesSent > 0) cout << "\nSent " << bytesSent <<" bytes";
	closesocket(client);
	WSACleanup();
}
