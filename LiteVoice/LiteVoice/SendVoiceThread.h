#pragma once

#include <QThread>
#include <WinSock2.h>
#include "ConnServer.h"
#include <vector>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
using namespace std;
class SendVoiceThread : public QThread
{
public:
	//int numClient;
	//vector <SOCKADDR_IN> addrs;
	SOCKADDR_IN addr;
	SOCKET sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SendVoiceThread();
	SendVoiceThread(char* addr);
	SendVoiceThread(char* addr, int port);
	~SendVoiceThread();
	void run();

	void addClient(char* addr);
	void addClient(char* addr, int port);
};
