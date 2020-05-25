#include "ChatUI.h"

ChatUI::ChatUI(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	isGroup = false;

	QWidget* widget = new QWidget();
	ui.scrollArea->setWidget(widget);
	layout = new QGridLayout();
	widget->setLayout(layout);
	messcount = 0;
}

ChatUI::~ChatUI()
{
}

void ChatUI::sendMyMess() {
	vector <Friend> friendsList = ConnServer::getFriends();

	QString mss = ui.edtlnMess->text();
	ui.edtlnMess->setText("");

	QDateTime dt = QDateTime::currentDateTime();
	QString dtString = QLocale("vn_VN").toString(dt, "d/MM/yyyy@hh:mm-AP");

	Message message;
	message.sender = "SownBanana";
	message.content = mss;
	message.attach = "";
	message.time = dtString;

	//owner.messages.push_back(message);
	if (isGroup) {
		ConnServer::getAGroup(name)->messages.push_back(message);
	}
	else {
		ConnServer::getAFriend(name)->messages.push_back(message);
	}

	//Add your message to QListWidget
	addYourMessage(message);

	std::string strmss = mss.toStdString();
	std::string strtime = dtString.toStdString();

	//wchar_t* x = (wchar_t*)malloc(mss.length() * sizeof(wchar_t));
	//mss.toWCharArray(x);
	char buf[1024];
	//wchar_t buf[1024];
	if (name == "ALL")
		sprintf(buf, "SEND ALL %s %s", strtime.c_str(), strmss.c_str());
	//swprintf_s(buf, sizeof(buf), L"SEND ALL %s", x);
	else if (isGroup)
		sprintf(buf, "SEND GROUP %s %s %s", name.toStdString().c_str(), strtime.c_str(), strmss.c_str());
	//swprintf_s(buf, sizeof(buf), L"SEND GROUP %s %s", name.toStdString().c_str(), x);
	else
		sprintf(buf, "SEND %s %s %s", name.toStdString().c_str(), strtime.c_str(), strmss.c_str());
	//swprintf_s(buf, sizeof(buf), L"SEND %s %s", name.toStdString().c_str(), x);

	ConnServer::sendServer(buf);
}

//void SingleChat::receivedMess(Member mem, QString mess, QString time) {
//	addFriendMessage(mem, mess, time);
//}
void ChatUI::on_avaicon_clicked(Member mem) {
	ui.edtlnMess->setText("avatar clicked");
}

void ChatUI::addYourMessage(Message mess) {
	QWidget* mssItem = new QWidget();
	//mssItem->setMaximumWidth(400);

	QString time = mess.time.split("@").at(1);
	time.replace("-", " ");
	QLabel* lblTime = new QLabel(time);

	lblTime->setStyleSheet("QLabel { color : white; }");
	lblTime->setAlignment(Qt::AlignRight);

	QLabel* txtChat = new QLabel(mess.content);
	txtChat->setStyleSheet("QLabel { background: #8b8d11;"
		" color: white;"
		" padding: 5px;"
		" border-radius: 10px;"
		" border-bottom-right-radius: 0;"
		" color: #fffaea;"
		" font: 9pt \"Segoe UI\";}");
	/*txtChat->setMaximumWidth(400);*/
	txtChat->setWordWrap(true);

	txtChat->setAlignment(Qt::AlignRight);

	QGridLayout* layoutt = new QGridLayout();
	layoutt->addWidget(lblTime, 1, 1);
	layoutt->addWidget(txtChat, 2, 1);

	/*layoutt->setAlignment(Qt::AlignRight);*/

	layoutt->setSizeConstraint(QLayout::SetFixedSize);
	mssItem->setLayout(layoutt);

	layout->addWidget(mssItem, messcount++, 1);
	ui.scrollArea->verticalScrollBar()->setValue(ui.scrollArea->verticalScrollBar()->maximum() + 100);
}

void ChatUI::addFriendMessage(Member mem, Message mess) {
	QWidget* mssItem = new QWidget();

	QString time = mess.time.split("@").at(1);
	time.replace("-", " ");
	QLabel* lblTime = new QLabel(time);
	//QLabel* lblTime = new QLabel("10:31 PM");
	//Connect signal
	//connect(lblTime, SIGNAL(click()), this, SLOT(on_avaicon_clicked({})));

	lblTime->setStyleSheet("QLabel { color : white; }");
	QLabel* txtChat = new QLabel(mess.content);
	txtChat->setStyleSheet("QLabel { background: #8b8d11;"
		" color: white;"
		" padding: 5px;"
		" border-radius: 10px;"
		" border-bottom-left-radius: 0;"
		" color: #fffaea;"
		" font: 9pt \"Segoe UI\";}");
	txtChat->setMaximumWidth(400);
	txtChat->setWordWrap(true);
	QGridLayout* layoutt = new QGridLayout();
	layoutt->addWidget(lblTime, 1, 1);
	layoutt->addWidget(txtChat, 2, 1);

	layoutt->setSizeConstraint(QLayout::SetFixedSize);

	mssItem->setLayout(layoutt);

	layout->addWidget(mssItem, messcount++, 0);

	ui.scrollArea->verticalScrollBar()->setValue(ui.scrollArea->verticalScrollBar()->maximum() + 100);
}