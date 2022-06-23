#pragma once

#include <QWidget>
#include "ui_AddFeatureSettingBox.h"

#include <qlabel.h>
#include <qgridlayout.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qgspointxy.h>

class AddFeatureSettingBox : public QWidget
{
	Q_OBJECT

public:
	AddFeatureSettingBox(QWidget *parent = Q_NULLPTR);
	AddFeatureSettingBox();
	AddFeatureSettingBox(QMap<QString, QVariant> settings);
	~AddFeatureSettingBox();

	void setSettings(QMap<QString, QVariant> settings);
	void setPoint(const QgsPointXY& p);

private:
	Ui::AddFeatureSettingBox ui;

	QGridLayout* mLayout;
	QList<QLabel*> labels;
	QList<QLineEdit*> lineEdits;
	QMap<QString, QVariant> mSettings;
	QToolButton* btnCancel;
	QToolButton* btnConfirm;
	QgsPointXY mPoint;

	void initUI();

signals:
	void settingCancel(bool cancel);
	void settingConfirm(QMap<QString, QVariant> settings);

private slots:
	void settingCancelTrigger();
	void settingConfirmTrigger();
};
