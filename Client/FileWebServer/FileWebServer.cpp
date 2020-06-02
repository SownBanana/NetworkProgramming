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
int checkFileType(char*); //0-text 1-img 2-video
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
	char htmlsrc[8192] = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n";
	char cmd[16], uri[256];
	char path[1024];
	sscanf(req, "%s %s", cmd, uri);
	if (uri[strlen(uri) - 1] == '/') {
		if (strcmp(uri, "/") == 0) {
			HANDLE h = FindFirstFileA("C:/*", &DATA);
			if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Đây là một thư mục
				sprintf(html, "%s<a href=\" /%s/ \">%s</a>", html, DATA.cFileName, DATA.cFileName);
			}
			else {
				// Đây là một file
				int check = checkFileType(DATA.cFileName);
				switch (check)
				{
				case 0: {
					sprintf(html, "%s<a href=\" /%s \" style=\"color: black;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
					break;
				}
				case 1: {
					sprintf(html, "%s<a href=\" /%s \" style=\"color: lightblue;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
					break;
				}
				case 2: {
					sprintf(html, "%s<a href=\" /%s \" style=\"color: brown;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
					break;
				}
				case 3: {
					sprintf(html, "%s<a href=\" /%s \" style=\"color: red;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
					break;
				}
				default:
					sprintf(html, "%s<a href=\" /%s \" style=\"color: gray;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
					break;
				}
			}

			while (FindNextFileA(h, &DATA)) {
				if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					// Đây là một thư mục
					sprintf(html, "%s<a href=\" /%s/ \">%s</a>", html, DATA.cFileName, DATA.cFileName);
				}
				else {
					// Đây là một file
					int check = checkFileType(DATA.cFileName);
					switch (check)
					{
					case 0: {
						sprintf(html, "%s<a href=\" /%s \" style=\"color: black;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
						break;
					}
					case 1: {
						sprintf(html, "%s<a href=\" /%s \" style=\"color: lightblue;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
						break;
					}
					case 2: {
						sprintf(html, "%s<a href=\" /%s \" style=\"color: brown;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
						break;
					}
					case 3: {
						sprintf(html, "%s<a href=\" /%s \" style=\"color: red;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
						break;
					}
					default:
						sprintf(html, "%s<a href=\" /%s \" style=\"color: gray;\" >%s</a>", html, DATA.cFileName, DATA.cFileName);
						break;
					}
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
				int check = checkFileType(DATA.cFileName);
				switch (check)
				{
				case 0: {
					sprintf(html, "%s<a href=\" %s%s \" style=\"color: black;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
					break;
				}
				case 1: {
					sprintf(html, "%s<a href=\" %s%s \" style=\"color: lightblue;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
					break;
				}
				case 2: {
					sprintf(html, "%s<a href=\" %s%s \" style=\"color: brown;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
					break;
				}
				case 3: {
					sprintf(html, "%s<a href=\" %s%s \" style=\"color: red;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
					break;
				}
				default:
					sprintf(html, "%s<a href=\" %s%s \" style=\"color: gray;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
					break;
				}
			}

			while (FindNextFileA(h, &DATA)) {
				if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					// Đây là một thư mục
					sprintf(html, "%s<a href=\" %s%s/ \">%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
				}
				else {
					// Đây là một file
					int check = checkFileType(DATA.cFileName);
					switch (check)
					{
					case 0: {
						sprintf(html, "%s<a href=\" %s%s \" style=\"color: black;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
						break;
					}
					case 1: {
						sprintf(html, "%s<a href=\" %s%s \" style=\"color: lightblue;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
						break;
					}
					case 2: {
						sprintf(html, "%s<a href=\" %s%s \" style=\"color: brown;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
						break;
					}
					case 3: {
						sprintf(html, "%s<a href=\" %s%s \" style=\"color: red;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
						break;
					}
					default:
						sprintf(html, "%s<a href=\" %s%s \" style=\"color: gray;\" >%s</a>", html, fafol, DATA.cFileName, DATA.cFileName);
						break;
					}
				}
			}
			send(client, html, strlen(html), 0);
		}
	}
	else {
		string strfafol = string(uri);
		replace(strfafol, "%20", " ");
		char* fafol = (char*)strfafol.c_str();
		sprintf(path, "C:%s", fafol);

		HANDLE h = FindFirstFileA(path, &DATA);

		int check = checkFileType(DATA.cFileName);
		switch (check)
		{
		case 0: {
			FILE* f = fopen(path, "r");
			if (f == NULL) {
				printf("Don't have file %s", path);
				break;
			}
			char* buf;
			fseek(f, 0, SEEK_END);
			long len = ftell(f);
			fseek(f, 0, SEEK_SET);
			buf = (char*)calloc(len + 1, sizeof(char));
			int count;
			count = fread(buf, 1, len, f);
			fclose(f);
			string txt(buf);
			replace(txt, "\n", "<br>");
			sprintf(htmlsrc, "%s<p>%s</p>", htmlsrc, txt.c_str());
			break;
		}
		case 1: {
			FILE* f = fopen(path, "rb");
			if (f == NULL) {
				printf("Don't have file %s", path);
				break;
			}
			char* buf;
			fseek(f, 0, SEEK_END);
			long len = ftell(f);
			fseek(f, 0, SEEK_SET);
			buf = (char*)calloc(len + 1, sizeof(char));
			int count;
			count = fread(buf, 1, len, f);
			fclose(f);
			//sprintf(htmlsrc, "%s<img src=\"C:%s\"/>", htmlsrc, fafol);
			sprintf(htmlsrc, "%s<img src = \"data:image/bmp,%s\" width = \"100\" height = \"100\" />", htmlsrc, buf);

			break;
		}
		case 2: {
			sprintf(htmlsrc, "%s <video src=\"C:%s\" controls></video>", htmlsrc, fafol);
			break;
		}
		case 3: {
			sprintf(htmlsrc, "%s<h style=\"color: red;\">%s is system file. You don't have permission</h>", htmlsrc, DATA.cFileName);
			break;
		}
		case 4: {
			break;
		}
		default:
			sprintf(htmlsrc, "%s<h style=\"color: red;\">This %s file format is not supported</h>", htmlsrc, DATA.cFileName);
			break;
		}
		send(client, htmlsrc, strlen(htmlsrc), 0);
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
int checkFileType(char* file) {
	char* ex = strstr(file, ".");
	if (ex == NULL) return 4;
	if ((strcmp(ex, ".sys") == 0)) return 3;
	if ((strcmp(ex, ".txt") == 0) || (strcmp(ex, ".log") == 0) || (strcmp(ex, ".cpp") == 0) || (strcmp(ex, ".java") == 0)) return 0;
	if ((strcmp(ex, ".png") == 0) || (strcmp(ex, ".jpg") == 0) || (strcmp(ex, ".gif") == 0) || (strcmp(ex, ".ico") == 0)) return 1;
	if ((strcmp(ex, ".mp4") == 0) || (strcmp(ex, ".webm") == 0) || (strcmp(ex, ".avi") == 0)) return 2;
	return 0;
}