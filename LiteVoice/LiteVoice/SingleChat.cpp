#include "SingleChat.h"
#include<QDebug>
SingleChat::SingleChat(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	isGroup = false;
	isCalling = false;
}

SingleChat::~SingleChat()
{
}

void SingleChat::sendMess() {
	vector <Friend> friendsList = ConnServer::getFriends();

	QString mss = ui.edtlnMess->text();
	if (mss == "") return;
	ui.edtlnMess->setText("");

	QDateTime dt = QDateTime::currentDateTime();

	QString dtString = QLocale("vn_VN").toString(dt, "dd/MM/yyyy@hh:mm-AP");

	string dtCStr = dtString.toStdString();

	//QString senddata = mss;

	Message message;
	message.sender = ConnServer::getMyName();
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

	//std::string x = senddata.toStdString();

	//const char* sendMessChar = x.c_str();

	//here
	//int x = sizeof(QString) * senddata.length();

	//char sendMessChar[1024];

	//memcpy(sendMessChar, &senddata, x);
	//here

	string mssStr = mss.toStdString();
	string nameStr = name.toStdString();
	char buf[1024];
	memset(buf, 0, 1024);
	//wchar_t buf[1024];
	//char* lenmss = (char*)calloc(4, sizeof(char));
	//itoa(x, lenmss, 10);
	if (name == "ALL") {
		sprintf(buf, "SEND ALL %s %s", dtCStr.c_str(), mssStr.c_str());
		ConnServer::sendServer(buf);
	}
	else if (isGroup) {
		sprintf(buf, "SEND GROUP %s %s %s", nameStr.c_str(), dtCStr.c_str(), mssStr.c_str());
		ConnServer::sendServer(buf);
	}
	else {
		sprintf(buf, "SEND %s %s %s", nameStr.c_str(), dtCStr.c_str(), mssStr.c_str());
	}
}

//void SingleChat::receivedMess(Member mem, QString mess, QString time) {
//	addFriendMessage(mem, mess, time);
//}
void SingleChat::on_avaicon_clicked(Member mem) {
	ui.edtlnMess->setText("avatar clicked");
}

void SingleChat::addYourMessage(Message mess) {
	QListWidgetItem* item = new QListWidgetItem();
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
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

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(lblTime, 0, 0);
	layout->addWidget(txtChat, 1, 0);

	layout->setColumnStretch(1, 600);

	layout->setAlignment(Qt::AlignRight);
	layout->setSizeConstraint(QLayout::SetFixedSize);

	int left, top, right, bott;
	QSize x = txtChat->size();

	//qDebug() << x;
	//layout->setContentsMargins(610 - txtChat->width(), 0, 0, 0);
	//layout->getContentsMargins(&left, &top, &right, &bott);

	mssItem->setLayout(layout);
	mssItem->setFixedWidth(600);

	item->setSizeHint(mssItem->sizeHint());

	ui.listWidget->addItem(item);

	//item->setTextAlignment(0x0002);
	ui.listWidget->setItemWidget(item, mssItem);
	ui.listWidget->scrollToBottom();
}

void SingleChat::addFriendMessage(Member mem, Message mess) {
	QListWidgetItem* item = new QListWidgetItem();
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
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
	QGridLayout* layout = new QGridLayout();
	layout->addWidget(lblTime, 1, 1);
	layout->addWidget(txtChat, 2, 1);

	layout->setSizeConstraint(QLayout::SetFixedSize);
	mssItem->setLayout(layout);

	item->setSizeHint(mssItem->sizeHint());

	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, mssItem);
	ui.listWidget->scrollToBottom();
	//Connect signal
	//connect(txtChat, SIGNAL(clicked()), this, SLOT(on_avaicon_clicked({})));
}

void SingleChat::makeCall() {
	if (!ConnServer::isCalling) {
		isCalling = true;
		ConnServer::setCalling(true);
		char buf[256];
		string nameStr = name.toStdString();
		sprintf(buf, "CALL %s %s", nameStr.c_str(), "9090");
		ConnServer::sendServer(buf);
		//rthread.start();
	}
}
void SingleChat::cancelCall() {
	if (ConnServer::isCalling) {
		isCalling = true;
		ConnServer::setCalling(false);
		char buf[256];
		string nameStr = name.toStdString();
		sprintf(buf, "CALL %s %s", nameStr.c_str(), "9090");
		//rthread.terminate();
	}
}