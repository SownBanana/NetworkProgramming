#pragma once

#include <QThread>

class CallThread : public QThread
{
	Q_OBJECT

public:
	CallThread(QObject *parent);
	~CallThread();
};
