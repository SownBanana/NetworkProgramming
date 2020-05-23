//#include "ConnectController.h"
//
//SOCKET ConnectController::client = 0;
//bool ConnectController::isLogined = false;
//vector <Group> ConnectController::groups = {};
//vector <Friend> ConnectController::friends = {};
//int ConnectController::numGroups = 0;
//int ConnectController::numFriends = 0;
//char* ConnectController::buf = (char*)calloc(1024, sizeof(char));
//wchar_t* ConnectController::wbuf = (wchar_t*)calloc(1024, sizeof(wchar_t));
//
//ConnectController::ConnectController(QObject* parent)
//	: QObject(parent)
//{
//	if (WSAStartup(MAKEWORD(2, 2), &wsa))
//	{
//		client = 0;
//	}
//	else {
//		client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//		SOCKADDR_IN server_addr;
//		server_addr.sin_family = AF_INET;
//		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//		server_addr.sin_port = htons(9000);
//		//Kết nối server
//		int ret = _WINSOCK2API_::connect(client, (SOCKADDR*)&server_addr, sizeof(server_addr));
//		if (ret == SOCKET_ERROR)
//		{
//			ret = WSAGetLastError();
//		}
//	}
//	getData();
//}
//
//ConnectController::~ConnectController()
//{
//	closesocket(client);
//	WSACleanup();
//}
//void ConnectController::sendServer(char* buf) {
//	send(client, buf, strlen(buf), 0);
//}
//void ConnectController::sendServer(wchar_t* buf) {
//	send(client, (char*)buf, wcslen(buf) * sizeof(wchar_t), 0);
//}
//int ConnectController::receiveServer() {
//	memset(buf, 0, sizeof(buf));
//	int ret = recv(client, buf, 1024, 0);
//	if (ret <= 0) return ret;
//	char cmd[32], stt[32], mss[256], tmp[32];
//	if (buf[ret - 1] == '\n') buf[ret - 1] = 0;
//	if (ret < sizeof(buf)) buf[ret] = 0;
//	int retc = sscanf(buf, "%s %s %s %s", cmd, stt, mss, tmp);
//
//	//CONNECT
//	if (strcmp(cmd, "CONNECT") == 0) {
//		if (strcmp(stt, "OK") == 0) {
//			ConnectController::isLogined = true;
//			const char* sendBuf = "LIST";
//			send(client, sendBuf, sizeof(sendBuf), 0);
//		}
//		else {
//		}
//	}
//	else if (strcmp(cmd, "MESSAGE_ALL") == 0) {
//		//User* u;
//		//for (int i = 0; i < groups.at(0).numUsers; i++) {
//		//	if (groups.at(0).usersList.at(i).name == stt) {
//		//		u = &groups.at(0).usersList.at(i);
//		//		break;
//		//	}
//		//}
//		emit on_mess_received(buf);
//	}
//	return ret;
//}
//SOCKET ConnectController::getSocket() {
//	return client;
//}
//
//bool  ConnectController::saveData(QString data) {
//	FILE* fdata = fopen("Resources/data/data.json", "w");
//	return true;
//}
//void ConnectController::getData() {
//	QFile loadFile(QStringLiteral("P:/Code4Life/NetworkProgramming/LiteVoice/data/data.json"));
//	if (!loadFile.open(QIODevice::ReadOnly)) {
//		qWarning("Couldn't open save file.");
//		return;
//	}
//	QByteArray saveData = loadFile.readAll();
//	qWarning(saveData);
//	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
//	QJsonObject json = loadDoc.object();
//
//	QJsonArray friendsArray = json["friends"].toArray();
//
//	for (int i = 0; i < friendsArray.size(); i++) {
//		Friend f;
//		QJsonObject aFriend = friendsArray[i].toObject();
//		f.read(aFriend);
//		ConnectController::friends.push_back(f);
//		ConnectController::numFriends++;
//	}
//
//	QJsonArray groupsArray = json["groups"].toArray();
//	for (int i = 0; i < groupsArray.size(); i++) {
//		Group g;
//		QJsonObject aGroup = groupsArray[i].toObject();
//		g.read(aGroup);
//		ConnectController::groups.push_back(g);
//		ConnectController::numGroups++;
//	}
//}
//vector <Friend> getFriendsList() {
//	vector <Friend> rs;
//	QFile loadFile(QStringLiteral("P:/Code4Life/NetworkProgramming/LiteVoice/data/data.json"));
//	if (!loadFile.open(QIODevice::ReadOnly)) {
//		qWarning("Couldn't open save file.");
//		return {};
//	}
//	QByteArray saveData = loadFile.readAll();
//	qWarning(saveData);
//	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
//	QJsonObject json = loadDoc.object();
//
//	QJsonArray friendsArray = json["friends"].toArray();
//
//	for (int i = 0; i < friendsArray.size(); i++) {
//		Friend f;
//		QJsonObject aFriend = friendsArray[i].toObject();
//		f.read(aFriend);
//		rs.push_back(f);
//	}
//	return rs;
//}
//vector <Group> getGroupsList() {
//	vector <Group> rs;
//	QFile loadFile(QStringLiteral("P:/Code4Life/NetworkProgramming/LiteVoice/data/data.json"));
//	if (!loadFile.open(QIODevice::ReadOnly)) {
//		qWarning("Couldn't open save file.");
//		return {};
//	}
//	QByteArray saveData = loadFile.readAll();
//	qWarning(saveData);
//	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
//	QJsonObject json = loadDoc.object();
//
//	QJsonArray friendsArray = json["friends"].toArray();
//
//	QJsonArray groupsArray = json["groups"].toArray();
//	for (int i = 0; i < groupsArray.size(); i++) {
//		Group g;
//		QJsonObject aGroup = groupsArray[i].toObject();
//		g.read(aGroup);
//		rs.push_back(g);
//	}
//	return rs;
//}