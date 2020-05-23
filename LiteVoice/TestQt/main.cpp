#include <QtCore/QCoreApplication>
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qfile.h"
#include "qjsondocument.h"
#include <QTextStream>

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);
	QFile loadFile(QStringLiteral("/data/data.json"));
	if (!loadFile.open(QIODevice::ReadOnly)) {
		qWarning("Couldn't open save file.");
		return false;
	}
	QByteArray saveData = loadFile.readAll();
	qWarning(saveData);
	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
	QJsonObject json = loadDoc.object();
	QJsonArray friendsArray = json["friends"].toArray();
	//QJsonObject aFriend = friendsArray[0].toObject();
	//QString name = aFriend["name"].toString();
	for (int i = 0; i < friendsArray.size(); i++) {
		QJsonObject aFriend = friendsArray[i].toObject();
		QJsonArray messArray = aFriend["messages"].toArray();
		for (int j = 0; j < messArray.size(); j++) {
			QJsonObject aMess = messArray[j].toObject();
			QString cnt = aMess["content"].toString();
			QTextStream(stdout) << cnt << endl;
		}
	}
	return a.exec();
}