#pragma once
#include "Member.h"
#include "User.h"
#include "Message.h"
#include <vector>
#include "qjsonobject.h"
#include "qjsonarray.h"
using namespace std;
class Group :
	public Member
{
public:
	vector <User> usersList;
	int numUsers;
	Group();

	void read(QJsonObject& aGroup);
	void write(QJsonObject& aGroup);
};
