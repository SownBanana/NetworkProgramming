// ChatClientUI.cpp : Defines the entry point for the application.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include "framework.h"
#include "ChatClientUI.h"

#pragma comment(lib, "ws2_32")

#define MAX_LOADSTRING 100

#define WM_SOCKET WM_USER + 1

typedef struct CLIENT {
	const char* id;
	bool isGroup;
	bool isBlock;
} CLIENT;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

CLIENT members[40];
CLIENT channels[80];

int numOfMember;
int numOfChannel;

SOCKET client;
//Đã đăng nhập chưa
bool isLogined = false;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CHATCLIENTUI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENTUI));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENTUI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CHATCLIENTUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 880, 490, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	//Tạo listbox view chat content
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 10, 10,
		160, 410, hWnd, (HMENU)IDC_LIST_CHATBOX, GetModuleHandle(NULL),
		NULL);
	//Tạo listbox view chat box
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 180, 10,
		500, 360, hWnd, (HMENU)IDC_LIST_CHATVIEW, GetModuleHandle(NULL),
		NULL);
	//Tạo edit text message
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, 180, 375, 540, 38, hWnd,
		(HMENU)IDC_EDIT_MESSAGE, GetModuleHandle(NULL), NULL);
	//Tạo listbox xem thành viên
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 690, 10,
		160, 360, hWnd, (HMENU)IDC_LIST_MEMBER, GetModuleHandle(NULL),
		NULL);
	//Tạo Button gửi tin nhắn
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("SEND"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 730,
		375, 120, 38, hWnd, (HMENU)IDC_BUTTON_SEND,
		GetModuleHandle(NULL), NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SendDlgItemMessageA(hWnd, IDC_LIST_MEMBER, LB_ADDSTRING, 0,
		(LPARAM)"Member List:");
	SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_ADDSTRING, 0,
		(LPARAM)"Channel:");

	//Khởi tạo wsa
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf_s("ERROR");
	}
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(9000);
	//Kết nối server
	int ret = connect(client, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		//printf_s("connect() failed: error code = %d", ret);
		return 1;
	}

	//Gắn nghe sự kiện
	WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

	numOfMember = 1;
	numOfChannel = 1;

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_BUTTON_SEND: {
			int ret;
			char buf[256];
			char sendBuf[256];

			char posi[256];
			char target[256];
			GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, buf, sizeof(buf));

			//Đã đăng nhập
			if (isLogined) {
				if (strcmp("DISCONNECT", buf) == 0)
					send(client, buf, sizeof(buf), 0);
				else {
					int channelId = SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_GETCURSEL, 0, 0);
					int memId = SendDlgItemMessageA(hWnd, IDC_LIST_MEMBER, LB_GETCURSEL, 0, 0);
					if (channelId <= 1) {
						sprintf(sendBuf, "%s %s", "SEND ALL", buf);
						send(client, sendBuf, sizeof(sendBuf), 0);
						sprintf(sendBuf, "%s : %s", "You", buf);
					}
					else {
						if (!channels[channelId].isGroup) {
							sprintf(sendBuf, "%s %s %s", "SEND", channels[channelId].id, buf);
							send(client, sendBuf, sizeof(sendBuf), 0);
							sprintf(sendBuf, "%s%s : %s", "You -> ", channels[channelId].id, buf);
						}
						else {
							sprintf(sendBuf, "%s %s %s", "SEND GROUP", channels[channelId].id, buf);
							send(client, sendBuf, sizeof(sendBuf), 0);
							sprintf(sendBuf, "%s%s : %s", "You -> Group ", channels[channelId].id, buf);
						}
					}
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)sendBuf);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
						SB_BOTTOM, 0);
				}
			}
			//Chưa đăng nhập => edit text để điền id
			else {
				sprintf(sendBuf, "%s %s", "CONNECT", buf);
				send(client, sendBuf, sizeof(sendBuf), 0);
			}
			SetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, "");
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam)) {
			DestroyWindow(hWnd);
			break;
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
			DestroyWindow(hWnd);
			break;
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			int ret;
			char buf[256];
			char cmd[32], stt[32], mss[256], tmp[32];
			ret = recv(wParam, buf, sizeof(buf), 0);
			if (buf[ret - 1] == '\n') buf[ret - 1] = 0;
			if (ret < sizeof(buf)) buf[ret] = 0;
			int retc = sscanf(buf, "%s %s %s %s", cmd, stt, mss, tmp);
			if (strcmp(cmd, "CONNECT") == 0) {
				if (strcmp(stt, "OK") == 0) {
					isLogined = true;
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)"Login Successfull!!!");
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_ADDSTRING, 0,
						(LPARAM)"World Channel");
					CLIENT worldChan;
					worldChan.id = "ALL";
					worldChan.isBlock = false;
					worldChan.isGroup = false;
					channels[numOfChannel++] = worldChan;

					const char* sendBuf = "LIST";
					send(client, sendBuf, sizeof(sendBuf), 0);
				}
				else {
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)(buf + 13));
				}
			}
			else if (strcmp(cmd, "DISCONNECT") == 0) {
				if (strcmp(stt, "OK") == 0) {
					isLogined = false;
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)"You Disconnected!");
				}
				else {
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)(buf + 16));
				}
			}
			else if (strcmp(cmd, "LIST") == 0) {
				//List user
				if (strcmp(stt, "OK") == 0) {
					char* user = strtok(buf + 8, " ");
					while (user) {
						if (user[strlen(user) - 1] == '\n') user[strlen(user) - 1] = 0;
						SendDlgItemMessageA(hWnd, IDC_LIST_MEMBER, LB_ADDSTRING, 0,
							(LPARAM)user);
						SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_ADDSTRING, 0,
							(LPARAM)user);

						members[numOfMember].id = (char*)malloc(strlen(user) + 1);
						memcpy((void*)members[numOfMember].id, user, strlen(user) + 1);
						members[numOfMember].isBlock = false;
						members[numOfMember].isGroup = false;
						numOfMember++;

						channels[numOfChannel].id = (char*)malloc(strlen(user) + 1);
						memcpy((void*)channels[numOfChannel].id, user, strlen(user) + 1);
						channels[numOfChannel].isBlock = false;
						channels[numOfChannel].isGroup = false;
						numOfChannel++;

						user = strtok(NULL, " ");
					}
				}
				else {
				}
			}
			else if (strcmp(cmd, "USER") == 0) {
				char sendBuf[256];
				if (strcmp(stt, "CONNECT") == 0) {
					members[numOfMember].id = (char*)malloc(strlen(mss) + 1);
					memcpy((void*)members[numOfMember].id, mss, strlen(mss) + 1);
					members[numOfMember].isBlock = false;
					members[numOfMember].isGroup = false;
					numOfMember++;

					channels[numOfChannel].id = (char*)malloc(strlen(mss) + 1);
					memcpy((void*)channels[numOfChannel].id, mss, strlen(mss) + 1);
					channels[numOfChannel].isBlock = false;
					channels[numOfChannel].isGroup = false;
					numOfChannel++;

					SendDlgItemMessageA(hWnd, IDC_LIST_MEMBER, LB_ADDSTRING, 0,
						(LPARAM)mss);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_ADDSTRING, 0,
						(LPARAM)mss);

					sprintf(sendBuf, "%s entered the Room!", mss);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)sendBuf);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
						SB_BOTTOM, 0);
				}
				else if (strcmp(stt, "DISCONNECT") == 0) {
					int index;
					for (index = 1; index < numOfMember; index++) {
						if (strcmp(members[index].id, mss) == 0) break;
					}
					if (index < numOfMember && index != 0) {
						SendDlgItemMessageA(hWnd, IDC_LIST_MEMBER, LB_DELETESTRING, index, 0);
						numOfMember -= 1;
					}
					for (index = 1; index < numOfChannel; index++) {
						if (strcmp(channels[index].id, mss) == 0) break;
					}
					if (index < numOfChannel && index != 0) {
						SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_DELETESTRING, index, 0);
						numOfChannel -= 1;
					}

					sprintf(sendBuf, "%s are leaving...", mss);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)sendBuf);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
						SB_BOTTOM, 0);
				}
			}
			else if (strcmp(cmd, "MESSAGE_ALL") == 0) {
				char sendBuf[256];
				sprintf(sendBuf, "%s : %s", stt, buf + 12 + strlen(stt));
				SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
					(LPARAM)sendBuf);
				SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
					SB_BOTTOM, 0);
			}
			else if (strcmp(cmd, "MESSAGE") == 0) {
				char sendBuf[256];
				sprintf(sendBuf, "%s -> You : %s", stt, buf + 8 + strlen(stt));
				SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
					(LPARAM)sendBuf);
				SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
					SB_BOTTOM, 0);
			}
			else if (strcmp(cmd, "MESSAGE_GROUP") == 0) {
				char sendBuf[256];
				sprintf(sendBuf, "%s -> %s : %s", mss, stt, buf + 15 + strlen(mss) + strlen(stt));
				SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
					(LPARAM)sendBuf);
				SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
					SB_BOTTOM, 0);
			}
			else if (strcmp(cmd, "GROUP") == 0) {
				if (retc == 3) {
					channels[numOfChannel].id = (char*)malloc(strlen(stt) + 1);
					memcpy((void*)channels[numOfChannel].id, stt, strlen(stt) + 1);
					channels[numOfChannel].isBlock = false;
					channels[numOfChannel].isGroup = true;
					numOfChannel++;

					SendDlgItemMessageA(hWnd, IDC_LIST_CHATBOX, LB_ADDSTRING, 0,
						(LPARAM)stt);

					sprintf(buf, "%s added you to Group ", mss, stt);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, LB_ADDSTRING, 0,
						(LPARAM)buf);
					SendDlgItemMessageA(hWnd, IDC_LIST_CHATVIEW, WM_VSCROLL,
						SB_BOTTOM, 0);
				}
			}
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_WRITE) {
		}
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}