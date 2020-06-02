#include "WorkerThread.h"

WorkerThread::WorkerThread()
{
}

WorkerThread::~WorkerThread()
{
}

char WorkerThread::buf[1024];

void WorkerThread::run() {
	//char buf[1024];
	while (true) {
		memset(buf, 0, sizeof(buf));
		int ret = recv(ConnServer::getClient(), buf, 1024, 0);
		if (ret < 0) return;

		char cmd[32], stt[32], mss[256], tmp[32];
		/*if (buf[ret - 1] == '\n') buf[ret - 1] = 0;
		if (ret < sizeof(buf)) buf[ret] = 0;*/
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
			//QString data = QString::fromLocal8Bit(buf);
			//QString b = "a";
			//QString messContent = "a";
			//char t[8];
			//memcpy(t, &messContent, 8);
			//char y[8];
			//memcpy(y, &messContent, 8);
			//int len = atoi(tmp);
			//int trsh = strlen(cmd) + strlen(stt) + strlen(mss) + strlen(tmp) + 4;
			//char x[8];
			//memcpy(x, buf + trsh, len);
			//memcpy(&messContent, x, len);
			//memcpy(&messContent, t, len);
			//memcpy(&messContent, y, len);
			//QString qbuf = QString::fromLocal8Bit(stt) + " " + QString::fromLocal8Bit(mss) + " |" + messContent;
			emit on_mess_received(buf);
		}
	}
}