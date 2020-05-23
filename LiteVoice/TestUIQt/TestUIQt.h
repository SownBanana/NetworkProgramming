#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TestUIQt.h"

class TestUIQt : public QMainWindow
{
	Q_OBJECT

public:
	TestUIQt(QWidget *parent = Q_NULLPTR);

private:
	Ui::TestUIQtClass ui;
};
