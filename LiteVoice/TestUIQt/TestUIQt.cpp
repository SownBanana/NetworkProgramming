#include "TestUIQt.h"
#include "aTabWidget.h"

TestUIQt::TestUIQt(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	aTabWidget* myTab = new aTabWidget();
	ui.myTabWidget->addTab(myTab, tr("Sown"));
}