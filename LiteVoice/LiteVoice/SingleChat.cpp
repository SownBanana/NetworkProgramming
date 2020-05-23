#include "SingleChat.h"

SingleChat::SingleChat(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	isGroup = false;
}

SingleChat::~SingleChat()
{
}

void SingleChat::sendMess() {
	QString mss = ui.edtlnMess->text();
	ui.edtlnMess->setText("");

	//Add your message to QListWidget
	addYourMessage(mss, "12:30 AM");

	std::string x = mss.toStdString();

	//wchar_t* x = (wchar_t*)malloc(mss.length() * sizeof(wchar_t));
	//mss.toWCharArray(x);
	char buf[1024];
	//wchar_t buf[1024];
	if (name == "ALL")
		sprintf(buf, "SEND ALL %s", x.c_str());
	//swprintf_s(buf, sizeof(buf), L"SEND ALL %s", x);
	else if (isGroup)
		sprintf(buf, "SEND GROUP %s %s", name.toStdString().c_str(), x.c_str());
	//swprintf_s(buf, sizeof(buf), L"SEND GROUP %s %s", name.toStdString().c_str(), x);
	else
		sprintf(buf, "SEND %s %s", name.toStdString().c_str(), x.c_str());
	//swprintf_s(buf, sizeof(buf), L"SEND %s %s", name.toStdString().c_str(), x);

	ConnServer::sendServer(buf);
}

//void SingleChat::receivedMess(Member mem, QString mess, QString time) {
//	addFriendMessage(mem, mess, time);
//}
void SingleChat::on_avaicon_clicked(Member mem) {
	ui.edtlnMess->setText("avatar clicked");
}

void SingleChat::addYourMessage(QString mess, QString time) {
	QListWidgetItem* item = new QListWidgetItem();
	QWidget* mssItem = new QWidget();
	//mssItem->setMaximumWidth(400);
	QLabel* lblTime = new QLabel("12:32 AM");

	lblTime->setStyleSheet("QLabel { color : white; }");
	lblTime->setAlignment(Qt::AlignRight);

	QLabel* txtChat = new QLabel(mess);
	txtChat->setStyleSheet("QLabel { background: #8b8d11;"
		" color: white;"
		" padding: 5px;"
		" border-radius: 6px;"
		" color: #fffaea;"
		" font: 9pt \"Segoe UI\";}");
	/*txtChat->setMaximumWidth(400);*/
	txtChat->setWordWrap(true);

	txtChat->setAlignment(Qt::AlignRight);

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(lblTime, 1, 1);
	layout->addWidget(txtChat, 2, 1);
	layout->setAlignment(Qt::AlignRight);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	mssItem->setLayout(layout);

	item->setSizeHint(mssItem->sizeHint());

	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, mssItem);
}

void SingleChat::addFriendMessage(Member mem, QString mess, QString time) {
	QListWidgetItem* item = new QListWidgetItem();
	QWidget* mssItem = new QWidget();

	QLabel* lblTime = new QLabel(time);
	//Connect signal
	//connect(lblTime, SIGNAL(click()), this, SLOT(on_avaicon_clicked({})));

	lblTime->setStyleSheet("QLabel { color : white; }");
	QLabel* txtChat = new QLabel(mess);
	txtChat->setStyleSheet("QLabel { background: #8b8d11;"
		" color: white;"
		" padding: 5px;"
		" border-radius: 6px;"
		" color: #fffaea;"
		" font: 9pt \"Segoe UI\";}");
	txtChat->setMaximumWidth(400);
	txtChat->setWordWrap(true);
	QGridLayout* layout = new QGridLayout();
	layout->addWidget(lblTime, 1, 1);
	layout->addWidget(txtChat, 2, 1);

	layout->setSizeConstraint(QLayout::SetFixedSize);
	mssItem->setLayout(layout);

	item->setSizeHint(mssItem->sizeHint());

	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, mssItem);

	//Connect signal
	//connect(txtChat, SIGNAL(clicked()), this, SLOT(on_avaicon_clicked({})));
}