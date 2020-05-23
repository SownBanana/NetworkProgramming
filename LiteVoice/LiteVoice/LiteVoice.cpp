#include "LiteVoice.h"

using namespace std;

LiteVoice::LiteVoice(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.tabWidgetMain->removeTab(0);
	ui.tabWidgetMain->removeTab(0);

	TabHome* tabHome = new TabHome();
	tabHome->setTabEnabled(3, false);
	ui.tabWidgetMain->addTab(tabHome, "Home");

	vector <Group> groupsList = ConnServer::getGroups();
	for (int i = 0; i < groupsList.size(); i++) {
		//Add group tab chat
		SingleChat* myTab = new SingleChat();
		myTab->name = groupsList.at(i).name;
		myTab->isGroup = true;
		ui.tabWidgetMain->addTab(myTab, tr(groupsList.at(i).name.toStdString().c_str()));
		addGroupChat(myTab);
		//Add group to groups list
		QListWidgetItem* item = new QListWidgetItem();
		QWidget* mssItem = new QWidget();

		QLabel* lblgrp = new QLabel(groupsList.at(i).name);
		QPushButton* btn = new QPushButton("...");
		lblgrp->setWordWrap(true);
		QGridLayout* layout = new QGridLayout();
		layout->addWidget(lblgrp, 1, 1);
		layout->addWidget(btn, 1, 4);

		layout->setSizeConstraint(QLayout::SetFixedSize);
		mssItem->setLayout(layout);

		item->setSizeHint(mssItem->sizeHint());

		QWidget* x = tabHome->widget(0);
		QString name = x->metaObject()->className();
		/*	->addItem(item);-
		ui.listWidget->setItemWidget(item, mssItem);*/
	}

	vector <Friend> friendsList = ConnServer::getFriends();
	for (int i = 0; i < friendsList.size(); i++) {
		SingleChat* myTab = new SingleChat();
		myTab->name = friendsList.at(i).name;
		ui.tabWidgetMain->addTab(myTab, tr(friendsList.at(i).name.left(1).toStdString().c_str()));
		addFriendChat(myTab);
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
void LiteVoice::addGroupChat(SingleChat* chat) {
	groupChats.push_back(chat);
}
void LiteVoice::addFriendChat(SingleChat* chat) {
	friendChats.push_back(chat);
}

void LiteVoice::receivedMess(QString data) {
	std::string x = data.toStdString();
	const char* buf = x.c_str();
	char cmd[32], stt[32], mss[256], tmp[32];
	int retc = sscanf(buf, "%s %s %s %s", cmd, stt, mss, tmp);
	User u;
	if (strcmp(cmd, "MESSAGE_ALL") == 0) {
		for (int i = 0; i < ConnServer::getGroups().at(0).numUsers; i++) {
			if (ConnServer::getGroups().at(0).usersList.at(i).name == stt) {
				u = ConnServer::getGroups().at(0).usersList.at(i);
				break;
			}
		}
		groupChats.at(0)->addFriendMessage(u, buf + 13 + strlen(stt), "01:16 PM");
	}
	else if (strcmp(cmd, "MESSAGE_GROUP") == 0) {
		Group* grp;
		int grpIndex;
		for (grpIndex = 0; grpIndex < ConnServer::getGroups().size(); grpIndex++) {
			if (ConnServer::getGroups().at(grpIndex).name == stt) { grp = &ConnServer::getGroups().at(grpIndex); break; }
		}
		for (int i = 0; i < grp->numUsers; i++) {
			if (grp->usersList.at(i).name == mss) {
				u = grp->usersList.at(i);
				break;
			}
		}
		groupChats.at(grpIndex)->addFriendMessage(u, buf + 16 + strlen(stt) + strlen(mss), "01:16 PM");
	}
	else if (strcmp(cmd, "MESSAGE") == 0) {
		Friend frd;
		int frdIndex;
		for (frdIndex = 0; frdIndex < ConnServer::getFriends().size(); frdIndex++) {
			if (ConnServer::getFriends().at(frdIndex).name == stt) { frd = ConnServer::getFriends().at(frdIndex); break; }
		}
		friendChats.at(frdIndex)->addFriendMessage(frd, buf + 9 + strlen(stt), "02:27 PM");
	}
}