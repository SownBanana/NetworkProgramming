#include "ConnServer.h"

using namespace std;

SOCKET ConnServer::client = 0;
bool ConnServer::isLogined = false;
vector <Group> ConnServer::groups = {};
vector <Friend> ConnServer::friends = {};
int ConnServer::numGroups = 0;
int ConnServer::numFriends = 0;
char* ConnServer::buf = (char*)calloc(1024, sizeof(char));
wchar_t* ConnServer::wbuf = (wchar_t*)calloc(1024, sizeof(wchar_t));

//Getter
vector <Friend> ConnServer::getFriends() {
	return friends;
};
vector <Group> ConnServer::getGroups() {
	return groups;
};
SOCKET ConnServer::getClient() {
	return client;
}

char* ConnServer::getBuf() {
	return buf;
}

ConnServer::ConnServer() {
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		client = 0;
	}
	else {
		client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		server_addr.sin_port = htons(9000);
		//Kết nối server
		int ret = connect(client, (SOCKADDR*)&server_addr, sizeof(server_addr));
		if (ret == SOCKET_ERROR)
		{
			ret = WSAGetLastError();
		}
	}
	getData();
}

ConnServer::~ConnServer() {
	closesocket(client);
	WSACleanup();
}
void ConnServer::sendServer(char* buf) {
	send(client, buf, strlen(buf), 0);
}
void ConnServer::sendServer(wchar_t* buf) {
	send(client, (char*)buf, wcslen(buf) * sizeof(wchar_t), 0);
}
int ConnServer::receiveServer() {
	memset(buf, 0, sizeof(buf));
	int ret = recv(client, buf, 1024, 0);
	if (ret <= 0) return ret;
	char cmd[32], stt[32], mss[256], tmp[32];
	if (buf[ret - 1] == '\n') buf[ret - 1] = 0;
	if (ret < sizeof(buf)) buf[ret] = 0;
	int retc = sscanf(buf, "%s %s %s %s", cmd, stt, mss, tmp);

	//CONNECT
	if (strcmp(cmd, "CONNECT") == 0) {
		if (strcmp(stt, "OK") == 0) {
			ConnServer::isLogined = true;
			const char* sendBuf = "LIST";
			send(client, sendBuf, sizeof(sendBuf), 0);
		}
		else {
		}
	}
	else if (strcmp(cmd, "MESSAGE_ALL") == 0) {
		//User* u;
		//for (int i = 0; i < groups.at(0).numUsers; i++) {
		//	if (groups.at(0).usersList.at(i).name == stt) {
		//		u = &groups.at(0).usersList.at(i);
		//		break;
		//	}
		//}
		//emit on_mess_received(buf);
	}
	return ret;
}
SOCKET ConnServer::getSocket() {
	return client;
}

bool ConnServer::isLogin() {
	return isLogined;
}

void ConnServer::setLogined(bool stt) {
	isLogined = stt;
}

bool  ConnServer::saveData(QString data) {
	FILE* fdata = fopen("Resources/data/data.json", "w");
	return true;
}
void ConnServer::getData() {
	QFile loadFile(QStringLiteral("P:/Code4Life/NetworkProgramming/LiteVoice/data/data.json"));
	if (!loadFile.open(QIODevice::ReadOnly)) {
		qWarning("Couldn't open save file.");
		return;
	}
	QByteArray saveData = loadFile.readAll();
	qWarning(saveData);
	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
	QJsonObject json = loadDoc.object();

	QJsonArray friendsArray = json["friends"].toArray();

	for (int i = 0; i < friendsArray.size(); i++) {
		Friend f;
		QJsonObject aFriend = friendsArray[i].toObject();
		f.read(aFriend);
		ConnServer::friends.push_back(f);
		ConnServer::numFriends++;
	}

	QJsonArray groupsArray = json["groups"].toArray();
	for (int i = 0; i < groupsArray.size(); i++) {
		Group g;
		QJsonObject aGroup = groupsArray[i].toObject();
		g.read(aGroup);
		ConnServer::groups.push_back(g);
		ConnServer::numGroups++;
	}
}
vector <Friend> getFriendsListJson() {
	vector <Friend> rs;
	QFile loadFile(QStringLiteral("P:/Code4Life/NetworkProgramming/LiteVoice/data/data.json"));
	if (!loadFile.open(QIODevice::ReadOnly)) {
		qWarning("Couldn't open save file.");
		return {};
	}
	QByteArray saveData = loadFile.readAll();
	qWarning(saveData);
	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
	QJsonObject json = loadDoc.object();

	QJsonArray friendsArray = json["friends"].toArray();

	for (int i = 0; i < friendsArray.size(); i++) {
		Friend f;
		QJsonObject aFriend = friendsArray[i].toObject();
		f.read(aFriend);
		rs.push_back(f);
	}
	return rs;
}
vector <Group> getGroupsListJson() {
	vector <Group> rs;
	QFile loadFile(QStringLiteral("P:/Code4Life/NetworkProgramming/LiteVoice/data/data.json"));
	if (!loadFile.open(QIODevice::ReadOnly)) {
		qWarning("Couldn't open save file.");
		return {};
	}
	QByteArray saveData = loadFile.readAll();
	qWarning(saveData);
	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
	QJsonObject json = loadDoc.object();

	QJsonArray friendsArray = json["friends"].toArray();

	QJsonArray groupsArray = json["groups"].toArray();
	for (int i = 0; i < groupsArray.size(); i++) {
		Group g;
		QJsonObject aGroup = groupsArray[i].toObject();
		g.read(aGroup);
		rs.push_back(g);
	}
	return rs;
}

Friend* ConnServer::getAFriend(QString name) {
	for (int i = 0; i < friends.size(); i++) {
		if (friends.at(i).name == name) return &friends.at(i);
	}
	return NULL;
}
Group* ConnServer::getAGroup(QString name) {
	for (int i = 0; i < groups.size(); i++) {
		if (groups.at(i).name == name) return &groups.at(i);
	}
	return NULL;
}