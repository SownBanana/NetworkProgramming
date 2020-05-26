#pragma once

#include <WinSock2.h>
#include "stdio.h"
#include "Group.h"
#include "Friend.h"
#include "qdatetime.h"
#include <vector>
#include "qstring.h"
#include "qjsondocument.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qfile.h"
#include "SingleChat.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
using namespace std;

//class SingleChat;
class ConnServer
{
	WSAData wsa;
	static char* myname;
	static bool isLogined;
	static SOCKET client;
	static vector <Group> groups;
	static vector <Friend> friends;
	static int numGroups;
	static int numFriends;
	static char* buf;
	static wchar_t* wbuf;
public:
	ConnServer();
	~ConnServer();

	//getter
	vector <Friend> static getFriends();
	vector <Group> static getGroups();
	SOCKET static getClient();
	static char* getBuf();
	void static getData();
	bool static isLogin();
	static char* getMyName();

	//Setter
	void static setMyName(char* name);
	void static setLogined(bool stt);

	bool static saveData(QString data);
	void static sendServer(char* buf);
	void static sendServer(char* buf, int length);
	void static sendServer(wchar_t* buf);
	int static receiveServer();

	static Friend* getAFriend(QString name);
	static Group* getAGroup(QString name);

	SOCKET static getSocket();
	vector <Friend> getFriendsListJson();
	vector <Group> getGroupsListJson();

	//signals:
	//	void static on_mess_received(char* buf);
};
