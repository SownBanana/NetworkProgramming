#include "TestUIQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	TestUIQt w;
	w.show();
	return a.exec();
}