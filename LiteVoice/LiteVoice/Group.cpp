#include "Group.h"

Group::Group() {
}
void Group::read(QJsonObject& aGroup)
{
	numUsers = 0;
	avaPath = aGroup["avatar"].toString();
	name = aGroup["name"].toString();
	QJsonArray messArray = aGroup["messages"].toArray();
	for (int j = 0; j < messArray.size(); j++) {
		Message m;
		QJsonObject aMess = messArray[j].toObject();
		m.content = aMess["content"].toString();
		m.attach = aMess["attach"].toString();
		m.sender = aMess["send"].toString();
		m.time = aMess["time"].toString();
		messages.push_back(m);
	}
	QJsonArray memArray = aGroup["members"].toArray();
	for (int j = 0; j < messArray.size(); j++) {
		User m;
		m.name = memArray[j].toObject()["name"].toString();
		m.role = memArray[j].toObject()["role"].toString();
		usersList.push_back(m);
		numUsers++;
	}
}

void Group::write(QJsonObject& json)
{
}