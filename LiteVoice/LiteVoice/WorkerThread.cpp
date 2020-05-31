#include "WorkerThread.h"

WorkerThread::WorkerThread()
{
}

WorkerThread::~WorkerThread()
{
}

char WorkerThread::buf[1024];

void WorkerThread::run() {
	bool isWaittingMess = false;
	//char buf[1024];
	while (true) {
		memset(buf, 0, sizeof(buf));
		int ret = recv(ConnServer::getClient(), buf, 1024, 0);
		if (ret < 0) return;
		if (!isWaittingMess) {
			char cmd[32], stt[32], mss[256], tmp[32];
			if (buf[ret - 1] == '\n') buf[ret - 1] = 0;
			if (ret < sizeof(buf)) buf[ret] = 0;
			int retc = sscanf(buf, "%s %s %s %s", cmd, stt, mss, tmp);

			//CONNECT
			if (strcmp(cmd, "CONNECT") == 0) {
				if (strcmp(stt, "OK") == 0) {
					ConnServer::setLogined(true);
					const char* sendBuf = "LIST";
					send(ConnServer::getClient(), sendBuf, sizeof(sendBuf), 0);
				}
				else {
				}
			}
			else if (strcmp(cmd, "MESSAGE_ALL") == 0 || strcmp(cmd, "MESSAGE") == 0 || strcmp(cmd, "MESSAGE_GROUP") == 0) {
				//isWaittingMess = true;
				QString data = QString::fromLocal8Bit(buf);
				emit on_messrq_received(data);
			}
		}
		else {
			emit on_mess_received();
		}
	}
}