#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "GraphicsWindowQt.h"

#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
#include <qgsinterpolator.h>
#include <qgsidwinterpolator.h>
#include <qgstininterpolator.h>
#include <qgsgridfilewriter.h>

#include <QList>
#include <QGroupBox>
#include <ctime>
#include <QStatusBar>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>

class TestSpatial : public QMainWindow
{
    Q_OBJECT

public:
    TestSpatial(QWidget *parent = Q_NULLPTR);
	~TestSpatial();

private:
    Ui::QtWidgetsApplication1Class ui;

private:
	QgsMapCanvas* mapCanvas;
	QList<QgsMapLayer*> layers;

	QLabel* lbl;

private:
	void InitUI();//界面初始化
	void InitQgs();
	void InitStatusBar();
	void IDWInterpolate();
	void TINInterpolate();
};
