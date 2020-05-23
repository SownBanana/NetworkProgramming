#pragma once

#include <QWidget>
#include "ui_MessageItem.h"
#include "Member.h"
#include "qstring.h"

class MessageItem : public QWidget
{
	Q_OBJECT

public:
	MessageItem(QWidget* parent = Q_NULLPTR);
	~MessageItem();
	void setMssItem(Member mem, QString mess, QString time);

private:
	Ui::MessageItem ui;
};
