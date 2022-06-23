#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "GraphicsWindowQt.h"

#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgsrasterlayer.h>
#include <qgsfilewidget.h>

#include <QList>
#include <QGroupBox>
#include <ctime>
#include <QStatusBar>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <qlayout.h>

class TestQgisGui : public QMainWindow
{
    Q_OBJECT

public:
    TestQgisGui(QWidget *parent = Q_NULLPTR);
	~TestQgisGui();

private:
    Ui::QtWidgetsApplication1Class ui;

private:
	QgsMapCanvas* mapCanvas;
	QList<QgsMapLayer*> layers;
	QHBoxLayout* hLayout;

private:
	void InitUI();//界面初始化
	void InitQgs();

	void testFileWidget();
	void testHighLight();
};
