// ChatServerWSAAsynSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <string>
using namespace std;

#define WM_SOCKET WM_USER + 1
#define WM_CLIENT WM_USER + 2

#pragma comment(lib, "ws2_32")

SOCKET clients[40];
char* ids[40];
int numberOfClients;

const char getClientIdMess[256] = "Enter your name (client_id: YOUR_NAME_HERE): ";
const char invalidMess[256] = "Your syntax invalid!\n ReEnter your name in right syntax (client_id: YOUR_NAME_HERE): ";

char* checkValidNameSyntax(char*);
void broadcastMess(SOCKET, const char*);
void removeClient(SOCKET);
SOCKET removeClientTmp(SOCKET);

BOOL CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int main()
{
	numberOfClients = 0;

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
	char buf[256];
	char sendBuf[256];

	if (wMsg == WM_SOCKET) {
		if (WSAGETSELECTERROR(lParam)) {
			closesocket(wParam);
			return true;
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT) {
			SOCKET client = accept(wParam, NULL, NULL);
			send(client, getClientIdMess, strlen(getClientIdMess), 0);
			WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_READ) {	//Dang nhap
			ret = recv(wParam, buf, sizeof(buf), 0);
			if (ret < sizeof(buf)) buf[ret] = 0;
			ids[numberOfClients] = new char[40];
			//Kiem tra ten
			char* check = checkValidNameSyntax(buf);
			if (check) {
				//Lay ten
				memcpy(ids[numberOfClients], check, strlen(check));
				ids[numberOfClients][strlen(check)] = 0;

				clients[numberOfClients++] = wParam;
				//Tra loi
				char wcmess[100];
				sprintf(wcmess, "Welcome %s to the Chatroom!\n", ids[numberOfClients - 1]);
				send(clients[numberOfClients - 1], wcmess, strlen(wcmess), 0);

				////Gửi thông báo đến các client khác
				sprintf(wcmess, "%s entered the Chatroom!\n", ids[numberOfClients - 1]);
				broadcastMess(clients[numberOfClients - 1], wcmess);
				WSAAsyncSelect(clients[numberOfClients - 1], hWnd, WM_CLIENT, FD_READ | FD_CLOSE);
			}
			else {
				//Ten ko hop le
				send(wParam, invalidMess, sizeof(invalidMess), 0);
			}
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
			closesocket(wParam);
		}
	}
	//Da dang nhap
	else if (wMsg == WM_CLIENT) {
		int index;
		for (index = 0; index < numberOfClients; index++) {
			if (clients[index] == wParam) break;
		}
		//Thoat
		if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
			const char* leaveMess = "is leaving the room.\n";
			sprintf(buf, "%s %s", ids[index], leaveMess);
			broadcastMess(wParam, buf);
			removeClient(wParam);
		}
		//Chuyen tiep tin
		else if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			ret = recv(wParam, buf, sizeof(buf), 0);
			if (ret < sizeof(buf)) buf[ret] = '\0';
			sprintf(sendBuf, "%s: %s", ids[index], buf);
			broadcastMess(wParam, sendBuf);
		}
	}
}

//Kiểm tra syntax
char* checkValidNameSyntax(char* buf) {
	char name[50];
	char syn[50];
	char tmp[50];
	const char* a = "client_id:";

	int ret = sscanf(buf, "%s %s", syn, name, tmp);
	if (ret != 2) return NULL;
	else if (strcmp(syn, a) == 0) return name;
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
		ids[i] = ids[numberOfClients - 1];
		numberOfClients -= 1;
	}
	closesocket(client);
}

void broadcastMess(SOCKET dst, const char* mess) {
	for (int i = 0; i <= numberOfClients; i++) {
		if (clients[i] != dst)
			send(clients[i], mess, strlen(mess), 0);
	}
}