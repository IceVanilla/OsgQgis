#include "AddFeatureSettingBox.h"
#include <qmessagebox.h>

AddFeatureSettingBox::AddFeatureSettingBox(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initUI();
}

AddFeatureSettingBox::AddFeatureSettingBox()
	: QWidget()
{
	ui.setupUi(this);
	initUI();
}

AddFeatureSettingBox::AddFeatureSettingBox(QMap<QString, QVariant> settings)
	: QWidget()
{
	ui.setupUi(this);
	setSettings(settings);
	initUI();
}

AddFeatureSettingBox::~AddFeatureSettingBox()
{
}

void AddFeatureSettingBox::setSettings(QMap<QString, QVariant> settings)
{
	//QMap<QString, QVariant> mMap;
	//mSettings.clear();
	//for (int i = 0; i < settings.size(); i++)
	//{
	//	mSettings.insert((settings.keys())[i], (settings.values())[i]);
	//}
	mSettings.clear();
	mSettings = settings;
}

void AddFeatureSettingBox::setPoint(const QgsPointXY& p)
{
	mPoint = p;
}

void AddFeatureSettingBox::initUI()
{
	mLayout = new QGridLayout();
	//QMessageBox::information(this, "", QString::number(mSettings.size()));
	int i = 0;
	for (QVariantMap::iterator itMap = mSettings.begin(); i < mSettings.size(); itMap++, i++)
	{
		QLabel* mLabel = new QLabel();
		mLabel->setText(itMap.key());
		labels.append(mLabel);
		QLineEdit* mEdit = new QLineEdit();
		mEdit->setText(itMap.value().toString());
		lineEdits.append(mEdit);
		mLayout->addWidget(mLabel, i, 0);
		mLayout->addWidget(mEdit, i, 1);
	}

	i++;
	btnCancel = new QToolButton();
	btnCancel->setText("cancel");
	this->connect(btnCancel, SIGNAL(clicked(bool)), this, SLOT(settingCancelTrigger()));
	mLayout->addWidget(btnCancel, i, 0);

	btnConfirm = new QToolButton();
	btnConfirm->setText("confirm");
	this->connect(btnConfirm, SIGNAL(clicked(bool)), this, SLOT(settingConfirmTrigger()));
	mLayout->addWidget(btnConfirm, i, 1);

	this->setLayout(mLayout);
}

void AddFeatureSettingBox::settingCancelTrigger()
{
	emit settingCancel(true);
	this->close();
}

void AddFeatureSettingBox::settingConfirmTrigger()
{
	for (int i = 0; i < labels.size(); i++)
	{
		mSettings[labels[i]->text()] = lineEdits[i]->text();
	}
	emit settingConfirm(mSettings);
	this->close();
}
