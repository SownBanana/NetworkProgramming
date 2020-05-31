#pragma once

#include <QThread>
#include <WinSock2.h>
#include "ConnServer.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
using namespace std;

class WorkerThread : public QThread
{
	Q_OBJECT

public:
	static char buf[1024];

public:
	WorkerThread();
	~WorkerThread();
	//char
	void run();

signals:
	void on_messrq_received(QString);
	void on_mess_received();
};
