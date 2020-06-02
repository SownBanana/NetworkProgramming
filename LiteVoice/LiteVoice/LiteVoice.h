#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LiteVoice.h"
#include "ConnServer.h"
#include "SingleChat.h"
#include "ChatUI.h"
#include "Friend.h"
#include <vector>
#include <QGridLayout>
#include "TabHome.h"
#include "ConnectController.h"
#include "WorkerThread.h"

class SingleChat;

class LiteVoice : public QMainWindow
{
	Q_OBJECT

public:
	vector <SingleChat*> groupChats;
	vector <SingleChat*> friendChats;

	QString waitProtocol;

	//vector <ChatUI*> groupChats;
	//vector <ChatUI*> friendChats;

public:
	LiteVoice(QWidget* parent = Q_NULLPTR);

	void addGroupChat(SingleChat* chat);
	void addFriendChat(SingleChat* chat);

	//void addGroupChat(ChatUI* chat);
	//void addFriendChat(ChatUI* chat);

private:
	Ui::LiteVoiceClass ui;

public slots:
	void receivedMessAll(QString);
	void receivedMess(QString);
};
