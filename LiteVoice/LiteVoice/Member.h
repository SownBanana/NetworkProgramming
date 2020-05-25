#pragma once
#include "qstring.h"
#include "Message.h"
#include <vector>

using namespace std;

class Message;

class Member
{
public:
	vector <Message> messages;
	QString name;
	QString avaPath;
};
