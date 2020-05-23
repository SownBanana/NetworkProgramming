#pragma once

#include <QTabWidget>
#include "ui_TabHome.h"

class TabHome : public QTabWidget
{
	Q_OBJECT

public:
	TabHome(QWidget *parent = Q_NULLPTR);
	~TabHome();

private:
	Ui::TabHome ui;
};
