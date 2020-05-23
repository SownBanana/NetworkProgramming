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
	WorkerThread();
	~WorkerThread();
	char buf[1024];
	void run();

signals:
	void on_mess_received(QString);
};
