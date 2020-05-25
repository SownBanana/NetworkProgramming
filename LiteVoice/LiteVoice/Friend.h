#pragma once
#include "Member.h"
#include "Message.h"
#include <vector>
#include "qstring.h"
#include "qstring.h"
#include "qjsondocument.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qfile.h"

using namespace std;
class Friend :
	public Member
{
public:
	int blockStatus;
	Friend();
	void read(QJsonObject& aFriend);
};
