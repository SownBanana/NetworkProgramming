#pragma once

#include <QWidget>
#include "ui_aTabWidget.h"

class aTabWidget : public QWidget
{
	Q_OBJECT

public:
	aTabWidget(QWidget *parent = Q_NULLPTR);
	~aTabWidget();

private:
	Ui::aTabWidget ui;
};
