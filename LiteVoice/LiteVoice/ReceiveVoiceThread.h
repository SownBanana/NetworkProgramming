#pragma once

#include <QThread>
#include <WinSock2.h>
#include "ConnServer.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
using namespace std;

class ReceiveVoiceThread : public QThread
{
public:
	SOCKET receiver;
	SOCKADDR_IN addr;
	ReceiveVoiceThread();
	~ReceiveVoiceThread();
	void run();
};
