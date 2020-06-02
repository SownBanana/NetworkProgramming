#pragma once

#include <QWidget>
#include "ui_SingleChat.h"
#include "ConnServer.h"
#include "qlistwidget.h"
#include "MessageItem.h"
#include "Member.h"
#include <vector>
#include "qgridlayout.h"
#include "qscrollbar.h"
#include "SendVoiceThread.h"
#include "ReceiveVoiceThread.h"
#include "LiteVoice.h"

class SingleChat : public QWidget
{
	Q_OBJECT

public:
	SingleChat(QWidget* parent = Q_NULLPTR);
	~SingleChat();
	//Member owner;
	QString name;
	bool isGroup;
	vector <Member> mem;
	void addYourMessage(Message mess);
	void addFriendMessage(Member mem, Message mess);
	bool isCalling;

	//ReceiveVoiceThread rthread;
	//SendVoiceThread sthread;

private:
	Ui::SingleChat ui;

private slots:
	void sendMess();
	void makeCall();
	void cancelCall();
	void on_avaicon_clicked(Member mem);
};
