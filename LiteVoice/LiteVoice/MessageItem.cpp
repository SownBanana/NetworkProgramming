#include "MessageItem.h"

MessageItem::MessageItem(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

MessageItem::~MessageItem()
{
}

void MessageItem::setMssItem(Member mem, QString mess, QString time) {
	ui.txtChat->setText(mess);
	ui.labelTime->setText(time);
}