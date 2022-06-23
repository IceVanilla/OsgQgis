#include "editFeatureStyleBox.h"
#include <qmessagebox.h>

EditFeatureStyleBox::EditFeatureStyleBox(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

EditFeatureStyleBox::EditFeatureStyleBox(QgsVectorLayer* layer)
{
	mLayer = layer;
	initUI();
}

EditFeatureStyleBox::~EditFeatureStyleBox()
{
}

void EditFeatureStyleBox::initUI()
{
	this->resize(500, 300);
	if (!mLayer->isValid())
	{
		return;
	}
	if (mLayer->geometryType() == QgsWkbTypes::GeometryType::PointGeometry)
	{
		initPointUI();
	}
	else if (mLayer->geometryType() == QgsWkbTypes::GeometryType::LineGeometry)
	{
		initLineUI();
	}
	else if (mLayer->geometryType() == QgsWkbTypes::GeometryType::PolygonGeometry)
	{
		initPolygonUI();
	}
	else
	{
		return;
	}
}

void EditFeatureStyleBox::initPointUI()
{
	pointIsSvg = false;
	mLayout = new QGridLayout();
	this->setLayout(mLayout);

	QLabel* lblColor = new QLabel("color");
	QPushButton* btnColor = new QPushButton();
	pointColor = QColor(Qt::black);
	QPalette pColor = btnColor->palette();
	pColor.setColor(QPalette::Button, pointColor);
	btnColor->setPalette(pColor);
	connect(btnColor, &QPushButton::clicked, this, [=]
		{
			pointColor = QColorDialog::getColor(pointColor, this);
			QPalette pColor = btnColor->palette();
			pColor.setColor(QPalette::Button, pointColor);
			btnColor->setPalette(pColor);
		});
	mLayout->addWidget(lblColor, 0, 0);
	mLayout->addWidget(btnColor, 0, 1);

	QLabel* lblFillColor = new QLabel("fillcolor");
	QPushButton* btnFillColor = new QPushButton();
	pointFillColor = QColor(Qt::white);
	QPalette pFillColor = btnFillColor->palette();
	pFillColor.setColor(QPalette::Button, pointFillColor);
	btnFillColor->setPalette(pFillColor);
	connect(btnFillColor, &QPushButton::clicked, this, [=]
		{
			pointFillColor = QColorDialog::getColor(pointFillColor, this);
			QPalette pFillColor = btnFillColor->palette();
			pFillColor.setColor(QPalette::Button, pointFillColor);
			btnFillColor->setPalette(pFillColor);
		});
	mLayout->addWidget(lblFillColor, 1, 0);
	mLayout->addWidget(btnFillColor, 1, 1);

	QLabel* lblSvg = new QLabel("svg");
	QPushButton* btnSvg = new QPushButton("select svg...");
	connect(btnSvg, &QPushButton::clicked, this, [=]
		{
			QString svgPath = QFileDialog::getOpenFileName(this, tr("Select"), "", tr("Files(*.svg)"));
			if (svgPath.isEmpty())
			{
				QMessageBox::information(this, "Failure", "No file selected.");
				return;
			}
			pointSvg = new QgsSvgMarkerSymbolLayer(svgPath);
			pointIsSvg = true;
		});
	mLayout->addWidget(lblSvg, 2, 0);
	mLayout->addWidget(btnSvg, 2, 1);

	btnCancel = new QPushButton("cancel");
	btnConfirm = new QPushButton("confirm");
	connect(btnConfirm, &QPushButton::clicked, this, [=]
		{
			if (pointIsSvg)
			{
				pointSvg->setColor(pointColor);
				pointSvg->setFillColor(pointFillColor);

				//创建QgsSymbolLayerList
				QgsSymbolLayerList symLayerList;
				symLayerList.append(pointSvg);
				//构建QgsMarkerSymbol
				QgsMarkerSymbol* markerSymbol = new QgsMarkerSymbol(symLayerList);
				//构建QgsSingleSymbolRenderer, 即最终图层的渲染器
				QgsSingleSymbolRenderer* symRenderer = new QgsSingleSymbolRenderer(markerSymbol);
				//设置当前图层的渲染器
				mLayer->setRenderer(symRenderer);
			}
			else
			{
				QgsSimpleMarkerSymbolLayer* pointMarker = new QgsSimpleMarkerSymbolLayer();
				pointMarker->setColor(pointColor);
				pointMarker->setFillColor(pointFillColor);

				QgsSymbolLayerList symLayerList;
				symLayerList.append(pointMarker);
				QgsMarkerSymbol* markerSymbol = new QgsMarkerSymbol(symLayerList);
				QgsSingleSymbolRenderer* symRenderer = new QgsSingleSymbolRenderer(markerSymbol);
				mLayer->setRenderer(symRenderer);
			}
			emit editFeatureStyleConfirm();
			this->close();
		});
	mLayout->addWidget(btnCancel, 3, 0);
	mLayout->addWidget(btnConfirm, 3, 1);
}

void EditFeatureStyleBox::initLineUI()
{
	mLayout = new QGridLayout();
	this->setLayout(mLayout);

	QLabel* lblColor = new QLabel("color");
	QPushButton* btnColor = new QPushButton();
	lineColor = QColor(Qt::black);
	QPalette pColor = btnColor->palette();
	pColor.setColor(QPalette::Button, lineColor);
	btnColor->setPalette(pColor);
	connect(btnColor, &QPushButton::clicked, this, [=]
		{
			lineColor = QColorDialog::getColor(lineColor, this);
			QPalette pColor = btnColor->palette();
			pColor.setColor(QPalette::Button, lineColor);
			btnColor->setPalette(pColor);
		});
	mLayout->addWidget(lblColor, 0, 0);
	mLayout->addWidget(btnColor, 0, 1);

	QLabel* lblWidth = new QLabel("width");
	lineWidth = 0.26;
	lineInputWidth = QString::number(lineWidth);
	QLineEdit* ledWidth = new QLineEdit(lineInputWidth);
	connect(ledWidth, &QLineEdit::textChanged, this, [=](QString text)
		{
			lineInputWidth = text;
		});
	mLayout->addWidget(lblWidth, 1, 0);
	mLayout->addWidget(ledWidth, 1, 1);

	btnCancel = new QPushButton("cancel");
	btnConfirm = new QPushButton("confirm");
	connect(btnConfirm, &QPushButton::clicked, this, [=]
		{
			lineWidth = lineInputWidth.toDouble();
			QgsSimpleLineSymbolLayer* lineRendererLayer = new QgsSimpleLineSymbolLayer();
			lineRendererLayer->setColor(lineColor);
			//lineRendererLayer->setColor(Qt::red);
			lineRendererLayer->setWidth(lineWidth);

			QgsSymbolLayerList symLayerList;
			symLayerList.append(lineRendererLayer);
			//QgsMarkerSymbol* markerSymbol = new QgsMarkerSymbol(symLayerList);
			QgsLineSymbol* lineSymbol = new QgsLineSymbol(symLayerList);
			QgsSingleSymbolRenderer* symRenderer = new QgsSingleSymbolRenderer(lineSymbol);
			mLayer->setRenderer(symRenderer);
			emit editFeatureStyleConfirm();
			//QMessageBox::information(this, "", QString::number(lineWidth));
			this->close();
		});
	mLayout->addWidget(btnCancel,2, 0);
	mLayout->addWidget(btnConfirm, 2, 1);
}

void EditFeatureStyleBox::initPolygonUI()
{
	polygonFillIsImg = false;
	mLayout = new QGridLayout();
	this->setLayout(mLayout);

	QLabel* lblFillColor = new QLabel("fillcolor");
	QPushButton* btnFillColor = new QPushButton();
	polygonFillColor = QColor(Qt::white);
	QPalette pColor = btnFillColor->palette();
	pColor.setColor(QPalette::Button, polygonFillColor);
	btnFillColor->setPalette(pColor);
	connect(btnFillColor, &QPushButton::clicked, this, [=]
		{
			polygonFillColor = QColorDialog::getColor(polygonFillColor, this);
			QPalette pColor = btnFillColor->palette();
			pColor.setColor(QPalette::Button, polygonFillColor);
			btnFillColor->setPalette(pColor);
		});
	mLayout->addWidget(lblFillColor, 0, 0);
	mLayout->addWidget(btnFillColor, 0, 1);

	QLabel* lblLineColor = new QLabel("linecolor");
	QPushButton* btnLineColor = new QPushButton();
	polygonLineColor = QColor(Qt::black);
	QPalette pFillColor = btnLineColor->palette();
	pFillColor.setColor(QPalette::Button, polygonLineColor);
	btnLineColor->setPalette(pFillColor);
	connect(btnLineColor, &QPushButton::clicked, this, [=]
		{
			polygonLineColor = QColorDialog::getColor(polygonLineColor, this);
			QPalette pFillColor = btnLineColor->palette();
			pFillColor.setColor(QPalette::Button, polygonLineColor);
			btnLineColor->setPalette(pFillColor);
		});
	mLayout->addWidget(lblLineColor, 1, 0);
	mLayout->addWidget(btnLineColor, 1, 1);

	QLabel* lblImg = new QLabel("image");
	QPushButton* btnImg = new QPushButton("select image...");
	connect(btnImg, &QPushButton::clicked, this, [=]
		{
			polygonImgPath = QFileDialog::getOpenFileName(this, tr("Select"), "", tr("Image Files(*.png)"));
			if (polygonImgPath.isEmpty())
			{
				QMessageBox::information(this, "Failure", "No file selected.");
				return;
			}
			polygonFillIsImg = true;
		});
	mLayout->addWidget(lblImg, 2, 0);
	mLayout->addWidget(btnImg, 2, 1);

	btnCancel = new QPushButton("cancel");
	btnConfirm = new QPushButton("confirm");
	connect(btnConfirm, &QPushButton::clicked, this, [=]
		{
			QgsSymbolLayerList symLayerList;
			if (polygonFillIsImg)
			{
				//QgsImageFillSymbolLayer* imgFillSymbolLayer = new QgsImageFillSymbolLayer();
				QgsRasterFillSymbolLayer* rasterFillSymbolLayer = new QgsRasterFillSymbolLayer;
				rasterFillSymbolLayer->setImageFilePath(polygonImgPath);
				QMessageBox::information(this, "", polygonImgPath);
				symLayerList.append(rasterFillSymbolLayer);

				QgsSimpleLineSymbolLayer* lineSymbolLayer = new QgsSimpleLineSymbolLayer();
				lineSymbolLayer->setColor(polygonLineColor);
				symLayerList.append(lineSymbolLayer);
			}
			else
			{
				QgsSimpleFillSymbolLayer* fillSymbolLayer = new QgsSimpleFillSymbolLayer();
				fillSymbolLayer->setFillColor(polygonFillColor);
				symLayerList.append(fillSymbolLayer);

				QgsSimpleLineSymbolLayer* lineSymbolLayer = new QgsSimpleLineSymbolLayer();
				lineSymbolLayer->setColor(polygonLineColor);
				symLayerList.append(lineSymbolLayer);
			}
			//QgsMarkerSymbol* markerSymbol = new QgsMarkerSymbol(symLayerList);
			QgsFillSymbol* fillSymbol = new QgsFillSymbol(symLayerList);
			QgsSingleSymbolRenderer* symRenderer = new QgsSingleSymbolRenderer(fillSymbol);
			mLayer->setRenderer(symRenderer);
			emit editFeatureStyleConfirm();
			this->close();
		});
	mLayout->addWidget(btnCancel, 3, 0);
	mLayout->addWidget(btnConfirm, 3, 1);
}
