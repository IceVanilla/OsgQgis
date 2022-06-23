#pragma once

#include <QWidget>
#include "ui_EditFeatureStyleBox.h"

#include <qgridlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qcolordialog.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <qlineedit.h>

#include <qgsmaplayer.h>
#include <qgsvectorlayer.h>
#include <qgsmarkersymbol.h>
#include <qgsmarkersymbollayer.h>
#include <qgssinglesymbolrenderer.h>
#include <qgslinesymbollayer.h>
#include <qgslinesymbol.h>
#include <qgsfillsymbollayer.h>
#include <qgsfillsymbol.h>

class EditFeatureStyleBox : public QWidget
{
	Q_OBJECT

public:
	EditFeatureStyleBox(QWidget *parent = Q_NULLPTR);
	EditFeatureStyleBox(QgsVectorLayer* layer);
	~EditFeatureStyleBox();

private:
	Ui::editFeatureStyleBox ui;

	QgsVectorLayer* mLayer;
	QGridLayout* mLayout;
	QPushButton* btnCancel;
	QPushButton* btnConfirm;

	QColor pointColor;
	QColor pointFillColor;
	bool pointIsSvg;
	QgsSvgMarkerSymbolLayer* pointSvg;

	QColor lineColor;
	QString lineInputWidth;
	double lineWidth;

	QColor polygonFillColor;
	QColor polygonLineColor;
	bool polygonFillIsImg;
	QString polygonImgPath;

	void initUI();
	void initPointUI();
	void initLineUI();
	void initPolygonUI();

signals:
	void editFeatureStyleConfirm();
};
