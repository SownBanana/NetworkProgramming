// TelnetServerWSAAsyncSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32")

#define WM_SOCKET WM_USER + 1
#define WM_CLIENT WM_USER + 2

FILE* fpout;
FILE* fpdata;

const char getLogMss[256] = "Enter your Username and Password (user_name pass_word): ";
const char wrongMss[256] = "Can't find your Username or Password\nReEnter (user_name pass_word):";
const char succMss[256] = "Login Successful!\nStart your command\n>";
const char* cmd = ">";

void removeClient(SOCKET);
SOCKET removeTmpClient(SOCKET);
bool checkLogin(char*, char*, FILE*);
BOOL CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

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

	// Khai báo cửa sổ
	WNDCLASS wndclass;
	const CHAR* providerClass = "AsyncSelect";
	HWND window;
	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)WinProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPCWSTR)providerClass;
	if (RegisterClass(&wndclass) == 0)
		return NULL;

	// Tạo cửa sổ
	if ((window = CreateWindow((LPCWSTR)providerClass, L"", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL)) == NULL)
		return NULL;

	WSAAsyncSelect(listener, window, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	printf("Waiting for a client\n");

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	WSACleanup();
}

BOOL CALLBACK WinProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	int ret;
	char buf[2048];

	//Cho client hoac client chua dang nhap
	if (wMsg == WM_SOCKET) {
		if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
			closesocket(wParam);
			return true;
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT) {
			SOCKET client = accept(wParam, NULL, NULL);
			printf("Client arrive\n");
			send(client, getLogMss, strlen(getLogMss), 0);
			WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			char username[50];
			char password[50];
			int ret = recv(wParam, buf, sizeof(buf), 0);
			ret = sscanf(buf, "%s %s", username, password);
			if (checkLogin(username, password, fpdata)) {
				//chap nhan dang nhap
				send(wParam, succMss, sizeof(succMss), 0);
				WSAAsyncSelect(wParam, hWnd, WM_CLIENT, FD_READ | FD_CLOSE);
			}
			//Dang nhap loi
			else {
				send(wParam, wrongMss, sizeof(wrongMss), 0);
			}
		}
	}
	//Da dang nhap
	else if (wMsg == WM_CLIENT) {
		if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
			closesocket(wParam);
			return true;
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			ret = recv(wParam, buf, sizeof(buf), 0);
			//Bỏ dấu \n
			if (buf[ret - 1] == '\n')
				buf[ret - 1] = 0;
			else if (ret < sizeof(buf)) buf[ret] = 0;
			strcat(buf, " > C:\\temp\\out.txt");
			system(buf);
			while (true) {
				ret = fread(buf, 1, sizeof(buf), fpout);
				if (ret <= 0) break;
				send(wParam, buf, ret, 0);
			}
			//Daau nhac lenh
			send(wParam, cmd, sizeof(cmd), 0);
			rewind(fpout);
		}
	}
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