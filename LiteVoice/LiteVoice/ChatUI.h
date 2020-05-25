#pragma once

#include <QWidget>
#include "ui_ChatUI.h"
#include "ConnServer.h"
#include "qlistwidget.h"
#include "MessageItem.h"
#include "Member.h"
#include <vector>
#include "qgridlayout.h"
#include "qscrollbar.h"

class ChatUI : public QWidget
{
	Q_OBJECT

public:
	ChatUI(QWidget* parent = Q_NULLPTR);
	~ChatUI();

	QString name;
	bool isGroup;
	vector <Member> mem;
	void addYourMessage(Message mess);
	void addFriendMessage(Member mem, Message mess);
	QGridLayout* layout;
	int messcount;

private:
	Ui::ChatUI ui;

private slots:

	void sendMyMess();
	void on_avaicon_clicked(Member mem);
};
