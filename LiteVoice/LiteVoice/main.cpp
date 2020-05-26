#include "ConnServer.h"
#include "ConnectController.h"
#include "LiteVoice.h"
#include <QtWidgets/QApplication>
#include "qobject.h"
#include "WorkerThread.h"

DWORD WINAPI ConnectionThread(LPVOID);
int main(int argc, char* argv[])
{
	//ConnectController conn;
	ConnServer conn;
	//conn.setMyName(argv[1]);
	conn.setMyName("SownBanana");
	char buf[1024];
	recv(conn.getClient(), buf, sizeof(buf), 0);
	sprintf(buf, "CONNECT %s", conn.getMyName());
	conn.sendServer(buf);

	QApplication a(argc, argv);
	LiteVoice w;
	w.show();

	WorkerThread wt;
	wt.start();

	//QObject::connect(&conn, ConnServer::on_mess_received, &w, LiteVoice::receivedMess);
	//CreateThread(0, 0, ConnectionThread, 0, 0, 0);
	QObject::connect(&wt, SIGNAL(on_mess_received(QString)), &w, SLOT(receivedMess(QString)));
	return a.exec();
}
DWORD WINAPI ConnectionThread(LPVOID lpParam) {
	while (true) {
		if (!ConnServer::receiveServer()) break;
	}
	return 0;
}