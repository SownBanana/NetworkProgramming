// FileWebServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <iostream>
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

WIN32_FIND_DATAA DATA;

void ProcessQueryString(SOCKET, char*);
void replace(string&, const string&, const string&);
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

	char buf[2048];
	// Chap nhan ket noi va truyen nhan du lieu
	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		int ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
		{
			closesocket(client);
			continue;
		}

		buf[ret] = 0;
		printf("%s\n", buf);

		ProcessQueryString(client, buf);

		closesocket(client);
	}

	closesocket(listener);
	WSACleanup();
}
void ProcessQueryString(SOCKET client, char* req)
{
	char html[8192] = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n"
		"<style>a{display: block; font-size: 20px; text-decoration: none; }</style>";
	char cmd[16], uri[256];
	char path[1024];
	sscanf(req, "%s %s", cmd, uri);
	if (strcmp(uri, "/") == 0) {
		HANDLE h = FindFirstFileA("C:/*", &DATA);
		if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Đây là một thư mục
			sprintf(html, "%s<a href=\" /%s/ \">%s</a>", html, DATA.cFileName, DATA.cFileName);
		}
		else {
			// Đây là một file
			sprintf(html, "%s<a href=\" /%s \">%s</a>", html, DATA.cFileName, DATA.cFileName);
		}

		while (FindNextFileA(h, &DATA)) {
			if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Đây là một thư mục
				sprintf(html, "%s<a href=\" /%s/ \">%s</a>", html, DATA.cFileName, DATA.cFileName);
			}
			else {
				// Đây là một file
				sprintf(html, "%s<a href=\" /%s \">%s</a>", html, DATA.cFileName, DATA.cFileName);
			}
		}
		send(client, html, strlen(html), 0);
	}
	else {
		string strfafol = string(uri);
		replace(strfafol, "%20", " ");
		char* fafol = (char*)strfafol.c_str();
		sprintf(path, "C:%s*.*", fafol);

		HANDLE h = FindFirstFileA(path, &DATA);
		if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Đây là một thư mục
			sprintf(html, "%s<a href=\" %s%s/ \">%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
		}
		else {
			// Đây là một file
			sprintf(html, "%s<a href=\" %s%s \">%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
		}

		while (FindNextFileA(h, &DATA)) {
			if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Đây là một thư mục
				sprintf(html, "%s<a href=\" %s%s/ \">%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
			}
			else {
				// Đây là một file
				sprintf(html, "%s<a href=\" %s%s \">%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
			}
		}
		send(client, html, strlen(html), 0);
	}
}
void replace(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}