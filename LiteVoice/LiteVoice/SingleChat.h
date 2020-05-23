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

class SingleChat : public QWidget
{
	Q_OBJECT

public:
	SingleChat(QWidget* parent = Q_NULLPTR);
	~SingleChat();
	QString name;
	bool isGroup;
	vector <Member> mem;
	void addYourMessage(QString mess, QString time);
	void addFriendMessage(Member mem, QString mess, QString time);

private:
	Ui::SingleChat ui;

private slots:

	void sendMess();
	void on_avaicon_clicked(Member mem);
};
