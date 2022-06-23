#include "QtWidgetsApplication1.h"
#include <osg/TexGen>
#include <qmessagebox.h>

#include <qgridlayout.h>
#include <qmessagebox.h>

TestQgisGui::TestQgisGui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	InitQgs();
	InitUI();
	testHighLight();
}

TestQgisGui::~TestQgisGui()
{
}

void TestQgisGui::InitUI()
{
	this->setCentralWidget(mapCanvas);
	//窗口最大化
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowTitle(QString::fromLocal8Bit("数字地球"));
}

void TestQgisGui::InitQgs()
{
	mapCanvas = new QgsMapCanvas();
	mapCanvas->setCanvasColor(QColor(255, 255, 255));
	mapCanvas->setVisible(true);
	mapCanvas->enableAntiAliasing(true);

	QString fileName = "C:/Users/COLORFUL/Documents/Data/osgearth/data/world.tif";
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(fileName, "world.tif", "gdal");
	if (!rasterLayer->isValid())
	{
		QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
		return;
	}
	QgsProject::instance()->addMapLayer(rasterLayer);
	mapCanvas->setExtent(rasterLayer->extent());
	layers.append(rasterLayer);
	mapCanvas->setLayers(layers);
	mapCanvas->setVisible(true);
	mapCanvas->freeze(false);
	mapCanvas->refresh();
}

void TestQgisGui::testFileWidget()
{
	QgsFileWidget* fileWidget = new QgsFileWidget();
	hLayout->addWidget(fileWidget);
	hLayout->update();
}

void TestQgisGui::testHighLight()
{
}
