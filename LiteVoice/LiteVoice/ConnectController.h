//#pragma once
//
//#include <QObject>
//#include <WinSock2.h>
//#include "stdio.h"
//#include "Group.h"
//#include "Friend.h"
//#include "qdatetime.h"
//#include <vector>
//#include "qstring.h"
//#include "qjsondocument.h"
//#include "qjsonarray.h"
//#include "qjsonobject.h"
//#include "qfile.h"
//
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//
//#pragma comment(lib, "ws2_32.lib")
//using namespace std;
//
//class ConnectController : public QObject
//{
//	Q_OBJECT
//
//public:
//	WSAData wsa;
//	static bool isLogined;
//	static SOCKET client;
//	static vector <Group> groups;
//	static vector <Friend> friends;
//	static int numGroups;
//	static int numFriends;
//	static char* buf;
//	static wchar_t* wbuf;
//
//	ConnectController(QObject* parent = Q_NULLPTR);
//	~ConnectController();
//
//	vector <Friend> static getFriends();
//	vector <Group> static getGroups();
//	SOCKET static getClient();
//	static char* getBuf();
//	void static getData();
//	bool static saveData(QString data);
//	void static sendServer(char* buf);
//	void static sendServer(wchar_t* buf);
//	int static receiveServer();
//
//	SOCKET static getSocket();
//	vector <Friend> getFriendsList();
//	vector <Group> getGroupsList();
//
//signals:
//	void static on_mess_received(char* buf);
//};
