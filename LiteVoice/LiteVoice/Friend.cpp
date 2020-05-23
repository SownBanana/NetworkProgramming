#include "Friend.h"

Friend::Friend() {};

void Friend::read(QJsonObject& aFriend) {
	name = aFriend["name"].toString();
	avaPath = aFriend["avatar"].toString();
	blockStatus = aFriend["isBlock"].toInt();
	QJsonArray messArray = aFriend["messages"].toArray();
	for (int j = 0; j < messArray.size(); j++) {
		Message m;
		QJsonObject aMess = messArray[j].toObject();
		m.content = aMess["content"].toString();
		m.attach = aMess["attach"].toString();
		m.sender = aMess["send"].toString();
		m.time = aMess["time"].toString();
		messages.push_back(m);
	}
}