#include "QtWidgetsApplication1.h"
#include <osg/TexGen>
#include <qmessagebox.h>

#include <qgridlayout.h>
#include <qmessagebox.h>

TestSpatial::TestSpatial(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	InitQgs();
	InitStatusBar();
	InitUI();
	//IDWInterpolate();
	TINInterpolate();
}

TestSpatial::~TestSpatial()
{
}

void TestSpatial::InitUI()
{
	// ui布局
	this->setCentralWidget(mapCanvas);
	//this->statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 不显示label的边框
	//this->statusBar()->setSizeGripEnabled(false); //不显示右边的大小控制点
	this->statusBar()->addWidget(lbl);
	//窗口最大化
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowTitle(QString::fromLocal8Bit("数字地球"));
}

void TestSpatial::InitQgs()
{
	mapCanvas = new QgsMapCanvas();
	mapCanvas->setCanvasColor(QColor(255, 255, 255));
	mapCanvas->setVisible(true);
	mapCanvas->enableAntiAliasing(true);

	QString fileName = "C:/Users/COLORFUL/Documents/Data/Spatial/ExtremePrecip_TibetanPlateau/ExtremePrecipIndicesshp/ExtremePrecipIndices.shp";
	QgsVectorLayer* vectorLayer = new QgsVectorLayer(fileName, "spatial", "ogr");
	if (!vectorLayer->isValid())
	{
		QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
		return;
	}
	QgsProject::instance()->addMapLayer(vectorLayer);
	mapCanvas->setExtent(vectorLayer->extent());
	layers.append(vectorLayer);
	mapCanvas->setLayers(layers);
	mapCanvas->setVisible(true);
	mapCanvas->freeze(false);
	mapCanvas->refresh();
}

void TestSpatial::InitStatusBar()
{
	lbl = new QLabel();
	if (layers.size() > 0)
	{
		QgsRectangle rec = layers[0]->extent();
		QString xmax = QString::number(rec.xMaximum());
		QString xmin = QString::number(rec.xMinimum());
		QString ymax = QString::number(rec.yMaximum());
		QString ymin = QString::number(rec.yMinimum());
		lbl->setText("xmax:" + xmax + ", xmin:" + xmin + ", ymax:" + ymax + ", ymin:" + ymin);
	}
}

void TestSpatial::IDWInterpolate()
{
	if (layers.size() > 0 && layers[0]->type() == QgsMapLayerType::VectorLayer)
	{
		QgsVectorLayer* vecLayer = (QgsVectorLayer*)layers[0];
		struct QgsInterpolator::LayerData layer1;
		layer1.interpolationAttribute = 7;
		layer1.source = vecLayer;
		layer1.sourceType = QgsInterpolator::SourceType::SourcePoints;
		layer1.valueSource = QgsInterpolator::ValueSource::ValueAttribute;
		QList<QgsInterpolator::LayerData> inputLayers;
		inputLayers.append(layer1);
		QgsIDWInterpolator* idw = new QgsIDWInterpolator(inputLayers);
		double r;
		if (idw->interpolatePoint(100, 30, r) == 0)
		{
			lbl->setText(QString::number(r));
		}
		//lbl->setText(QString::number(idw->interpolatePoint(100, 30, r)));

		int rows = (vecLayer->extent().xMaximum() - vecLayer->extent().xMinimum()) / 0.1;
		int columns = (vecLayer->extent().yMaximum() - vecLayer->extent().yMinimum()) / 0.1;
		QgsGridFileWriter* gridWriter = new QgsGridFileWriter(idw, "C:/Users/COLORFUL/Documents/Data/Spatial/test/idw_output.tif", vecLayer->extent(), rows, columns);
		lbl->setText(QString::number(gridWriter->writeFile()));
	}
	//QgsVectorLayer* vecLayer = (QgsVectorLayer*)layers[0];
	//lbl->setText(vecLayer->attributeDisplayName(7));
}

void TestSpatial::TINInterpolate()
{
	if (layers.size() > 0 && layers[0]->type() == QgsMapLayerType::VectorLayer)
	{
		QgsVectorLayer* vecLayer = (QgsVectorLayer*)layers[0];
		struct QgsInterpolator::LayerData layer1;
		layer1.interpolationAttribute = 7;
		layer1.source = vecLayer;
		layer1.sourceType = QgsInterpolator::SourceType::SourcePoints;
		layer1.valueSource = QgsInterpolator::ValueSource::ValueAttribute;
		QList<QgsInterpolator::LayerData> inputLayers;
		inputLayers.append(layer1);
		QgsTinInterpolator* tin = new QgsTinInterpolator(inputLayers);
		double r;
		if (tin->interpolatePoint(100, 30, r, nullptr) == 0)
		{
			lbl->setText(QString::number(r));
		}
		//lbl->setText(QString::number(idw->interpolatePoint(100, 30, r)));

		int rows = (vecLayer->extent().xMaximum() - vecLayer->extent().xMinimum()) / 0.1;
		int columns = (vecLayer->extent().yMaximum() - vecLayer->extent().yMinimum()) / 0.1;
		QgsGridFileWriter* gridWriter = new QgsGridFileWriter(tin, "C:/Users/COLORFUL/Documents/Data/Spatial/test/tin_output.tif", vecLayer->extent(), rows, columns);
		lbl->setText(QString::number(gridWriter->writeFile()));
	}
	//QgsVectorLayer* vecLayer = (QgsVectorLayer*)layers[0];
	//lbl->setText(vecLayer->attributeDisplayName(7));
}
