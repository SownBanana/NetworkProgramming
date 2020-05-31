#include "SingleChat.h"
#include<QDebug>
SingleChat::SingleChat(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	isGroup = false;

	//ui.listWidget->setStyleSheet(
	//	" QScrollBar:vertical {    "
	//	"     border: none;"
	//	"     background: black;"
	//	"     width:3px;"
	//	"     margin: 0px 0px 0px 0px;"
	//	"   }"
	//	"   QScrollBar::handle:vertical {"
	//	"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
	//	"    stop: 0 rgb(32, 32, 32), stop: 0.5 rgb(32, 32, 32), stop:1 rgb(32, 32, 32));"
	//	"      min-height: 0px;"
	//	" }"
	//	"    QScrollBar::add-line:vertical {"
	//	"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
	//	"    stop: 0 rgb(32, 32, 32), stop: 0.5 rgb(32, 32, 32), stop:1 rgb(32, 32, 32));"
	//	"     height: 0px;"
	//	"     subcontrol-position: bottom;"
	//	"     subcontrol-origin: margin;"
	//	"  }"
	//	"     QScrollBar::sub-line:vertical {"
	//	"        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
	//	"    stop: 0 rgb(32, 32, 32), stop: 0.5 rgb(32, 32, 32), stop:1 rgb(32, 32, 32));"
	//	"      height: 0 px;"
	//	"     subcontrol-position: top;"
	//	"     subcontrol-origin: margin;"
	//	"  }"
	//	"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: black;}"

	//	"QListWidget{border: none;"
	//	//"background: #121212;"
	//	"} "
	//	"QListWidget::item{border: none;"
	//	//"background: #121212;"
	//	"fontre: 9pt \"Segoe UI\";} "
	//	"QListWidget::item:hover{background: transparent;} "
	//	"QListWidget::item:disabled{background: transparent;}");
}

SingleChat::~SingleChat()
{
}

void SingleChat::sendMess() {
	vector <Friend> friendsList = ConnServer::getFriends();

	QString mss = ui.edtlnMess->text();
	ui.edtlnMess->setText("");

	QDateTime dt = QDateTime::currentDateTime();

	QString dtString = QLocale("vn_VN").toString(dt, "dd/MM/yyyy@hh:mm-AP");

	string dtCStr = dtString.toStdString();

	QString senddata = mss;

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

	int x = sizeof(QString) * senddata.length();
	char sendMessChar[1024];
	//sendMessChar = new char[x];
	memcpy(sendMessChar, &senddata, x);

	//2QByteArray sendMessChar = mss.toUtf8();

	//3wchar_t* x = (wchar_t*)malloc(mss.length() * sizeof(wchar_t));
	//mss.toWCharArray(x);
	char buf[1024];
	//wchar_t buf[1024];
	if (name == "ALL") /*{
		memcpy(buf, "SEND ALL ", 9);
		memcpy(buf + 9, sendMessChar, 8);
	}*/
	//sprintf(buf, "SEND ALL %s", sendMessChar);
	{
		sprintf(buf, "SEND ALL %s %d", dtCStr.c_str(), x);
		ConnServer::sendServer(buf);
		ConnServer::sendServer(sendMessChar);
	}
	else if (isGroup)
		sprintf(buf, "SEND GROUP %s %s", name.toStdString().c_str(), sendMessChar);
	//swprintf_s(buf, sizeof(buf), L"SEND GROUP %s %s", name.toStdString().c_str(), x);
	else
		sprintf(buf, "SEND %s %s", name.toStdString().c_str(), sendMessChar);
	//swprintf_s(buf, sizeof(buf), L"SEND %s %s", name.toStdString().c_str(), x);

	//Message test;
	//test.content = buf;
	//test.time = "test@test-test";
	//addYourMessage(test);

	//ConnServer::sendServer(buf);
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