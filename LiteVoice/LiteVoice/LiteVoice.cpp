#include "LiteVoice.h"

using namespace std;

LiteVoice::LiteVoice(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.lnedtSearch->setText(ConnServer::getMyName());

	ui.tabWidgetMain->removeTab(0);
	ui.tabWidgetMain->removeTab(0);

	TabHome* tabHome = new TabHome();
	tabHome->setTabEnabled(3, false);
	ui.tabWidgetMain->addTab(tabHome, "Home");

	QListWidget* lstFrds;
	QListWidget* lstGrps;
	QWidget* px = tabHome->widget(0);
	QList<QListWidget*> alllistWgs = px->findChildren<QListWidget*>();
	if (alllistWgs.count() == 2) {
		lstFrds = alllistWgs[0];
		lstGrps = alllistWgs[1];
	}

	vector <Group> groupsList = ConnServer::getGroups();
	for (int i = 0; i < groupsList.size(); i++) {
		//Add group tab chat
		SingleChat* myTab = new SingleChat();
		//ChatUI* myTab = new ChatUI();
		//myTab->owner = groupsList.at(i);
		myTab->name = groupsList.at(i).name;
		myTab->isGroup = true;
		ui.tabWidgetMain->addTab(myTab, tr(groupsList.at(i).name.toStdString().c_str()));
		addGroupChat(myTab);

		//Add group to groups list
		QListWidgetItem* item = new QListWidgetItem();
		QWidget* grpItem = new QWidget();

		QLabel* lblgrp = new QLabel(groupsList.at(i).name);
		QPushButton* btn = new QPushButton("...");
		lblgrp->setWordWrap(true);
		QGridLayout* layout = new QGridLayout();
		layout->addWidget(lblgrp, 1, 1);
		layout->addWidget(btn, 1, 4);

		layout->setSizeConstraint(QLayout::SetFixedSize);
		grpItem->setLayout(layout);

		item->setSizeHint(grpItem->sizeHint());

		lstGrps->addItem(item);
		lstGrps->setItemWidget(item, grpItem);
	}

	vector <Friend> friendsList = ConnServer::getFriends();
	for (int i = 0; i < friendsList.size(); i++) {
		SingleChat* myTab = new SingleChat();
		//ChatUI* myTab = new ChatUI();
		//myTab->owner = friendsList.at(i);
		myTab->name = friendsList.at(i).name;
		ui.tabWidgetMain->addTab(myTab, tr(friendsList.at(i).name.left(1).toStdString().c_str()));
		addFriendChat(myTab);

		//Add friend to friends list
		QListWidgetItem* item = new QListWidgetItem();
		QWidget* frdItem = new QWidget();

		QLabel* lblgrp = new QLabel(friendsList.at(i).name);
		QPushButton* btn = new QPushButton("...");
		lblgrp->setWordWrap(true);
		QGridLayout* layout = new QGridLayout();
		layout->addWidget(lblgrp, 0, 0);
		layout->addWidget(btn, 0, 1);
		layout->setColumnStretch(1, 50);

		layout->setSizeConstraint(QLayout::SetFixedSize);
		frdItem->setLayout(layout);

		item->setSizeHint(frdItem->sizeHint());

		lstFrds->addItem(item);
		lstFrds->setItemWidget(item, frdItem);
	}

	//QWidget* pWidget = (QWidget*)ui.tabWidgetMain->widget(0);
	//QString x = pWidget->metaObject()->className();
	//if (pWidget->metaObject()->className() == "QWidget") {
	//	QGridLayout* htlayout;
	//	TabHome* tabHome = new TabHome(pWidget);
	//	htlayout->addWidget(tabHome, 0, 0);
	//	pWidget->setLayout(htlayout);
	//}
}

//void LiteVoice::addGroupChat(ChatUI* chat) {
//	groupChats.push_back(chat);
//}
//void LiteVoice::addFriendChat(ChatUI* chat) {
//	friendChats.push_back(chat);
//}

void LiteVoice::addGroupChat(SingleChat* chat) {
	groupChats.push_back(chat);
}
void LiteVoice::addFriendChat(SingleChat* chat) {
	friendChats.push_back(chat);
}

void LiteVoice::receivedMess(QString data) {
	std::string x = data.toStdString();
	const char* buf = x.c_str();
	char cmd[32], stt[32], mss[256], time[50], tmp[50];
	int retc = sscanf(buf, "%s %s %s %s", cmd, stt, mss, time, tmp);
	User u;

	if (strcmp(cmd, "MESSAGE_ALL") == 0) {
		//QString qstrrcv;
		//int x = strlen(mss);
		//memcpy(&qstrrcv, mss, sizeof(qstrrcv));

		Message message;
		message.sender = stt;
		message.time = mss;
		message.content = buf + 14 + strlen(stt) + strlen(mss);

		/*message.content = qstrrcv.split(" ").at(1);
		message.time = qstrrcv.split(" ").at(0);*/

		for (int i = 0; i < ConnServer::getGroups().at(0).numUsers; i++) {
			if (ConnServer::getGroups().at(0).usersList.at(i).name == stt) {
				u = ConnServer::getGroups().at(0).usersList.at(i);
				break;
			}
		}

		groupChats.at(0)->addFriendMessage(u, message);

		ConnServer::getGroups().at(0).messages.push_back(message);
	}
	else if (strcmp(cmd, "MESSAGE_GROUP") == 0) {
		//QString qstrmess;
		//memcpy(&qstrmess, buf + 16 + strlen(stt) + strlen(mss), sizeof(qstrmess));

		Message message;
		message.sender = mss;
		message.content = buf + 17 + strlen(stt) + strlen(mss) + strlen(time);
		message.time = time;

		//message.content = qstrmess.split(" ").at(1);

		Group* grp = ConnServer::getAGroup(stt);
		grp->messages.push_back(message);

		int grpIndex;
		for (grpIndex = 0; grpIndex < ConnServer::getGroups().size(); grpIndex++) {
			if (ConnServer::getGroups().at(grpIndex).name == stt) {
				break;
			}
		}
		for (int i = 0; i < grp->numUsers; i++) {
			if (grp->usersList.at(i).name == mss) {
				u = grp->usersList.at(i);
				break;
			}
		}

		groupChats.at(grpIndex)->addFriendMessage(u, message);
	}
	else if (strcmp(cmd, "MESSAGE") == 0) {
		//QString qstrmess;

		//QString tst = "Alo alo";
		//char a[sizeof(QString)];
		//memcpy(a, &tst, sizeof(tst));
		//memcpy(&qstrmess, a, sizeof(qstrmess));

		//memcpy(&qstrmess, buf + 9 + strlen(stt), sizeof(qstrmess));

		Message message;
		message.sender = stt;

		message.content = buf + 10 + strlen(stt) + strlen(mss);

		message.time = mss;

		/*message.content = qstrmess.split(" ").at(1);
		message.time = qstrmess.split(" ").at(0);*/

		Friend* frd = ConnServer::getAFriend(stt);
		frd->messages.push_back(message);

		int frdIndex;
		for (frdIndex = 0; frdIndex < ConnServer::getFriends().size(); frdIndex++) {
			if (ConnServer::getFriends().at(frdIndex).name == stt) {
				break;
			}
		}

		friendChats.at(frdIndex)->addFriendMessage(*frd, message);
	}
}