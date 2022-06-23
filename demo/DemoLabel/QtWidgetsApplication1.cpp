#include "QtWidgetsApplication1.h"
#include <qmenubar.h>       // QT�˵����⣬��Ų˵�
#include <qmessagebox.h>    // QT��Ϣ���ӣ���ʾ������ʾ
#include <qfiledialog.h>    // QT�ļ�Ŀ¼�⣬���ļ�ѡ�񴰿�
#include <qinputdialog.h>

#include <qgslayertreemodel.h>  // ʹ�ø����������µ���ģ�ͣ�һ����QgsLayerTreeViewһ��ʹ��
#include <qgridlayout.h>        // դ�񲼾ֹ�����

#include "qtoolbutton.h"
#include "qdockwidget.h"
#include "qgslayertreeregistrybridge.h"

#include <qgslayertreegroup.h>
#include <qgsmemoryproviderutils.h>
#include <qgsannotationlayer.h>
#include <qgsmarkersymbol.h>
#include <qgsmarkersymbollayer.h>
#include <qgsannotationmarkeritem.h>
#include <qgsogrproviderutils.h>
#include <qgsvectorfilewritertask.h>
#include <qgsapplication.h>

#include <QTime>
DemoLabel* DemoLabel::sm_instance = nullptr;

DemoLabel::DemoLabel(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	InitUI();
}

DemoLabel::~DemoLabel()
{
}

// �ۺ������򿪲���ʾդ������
void DemoLabel::openRasterFileActionTrigger()
{
    // ���ļ�ѡ��Ի���
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster file"), "", "remote sensing image(*.tif *.tiff);;image(*.jpg *.jpeg *.png *.bmp)");
    if (fileName.isNull())
    {
        return;
    }
    QStringList temp = fileName.split('/');
    QString basename = temp.at(temp.size() - 1);

    // ����QgsrRasterLayer��
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(fileName, basename, "gdal");
    if (!rasterLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
        return;
    }

    // ���դ������
    QgsProject::instance()->addMapLayer(rasterLayer);   // ע��
    mapCanvas->setExtent(rasterLayer->extent());        // ��������Χ����Ϊդ��ͼ�㷶Χ
    layers.append(rasterLayer);                         // ��դ��ͼ��׷�ӵ�������
    mapCanvas->setLayers(layers);                       // ��ͼ�㻭��������
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();                               // ���»���
}

// �ۺ������򿪲���ʾʸ������
void DemoLabel::openVectorFileActionTrigger()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open shape file"), "", "*.shp");
    if (fileName.isNull())
    {
        return;
    }
    QStringList temp = fileName.split('/');
    QString basename = temp.at(temp.size() - 1);

    QgsVectorLayer* vecLayer = new QgsVectorLayer(fileName, basename, "ogr");
    if (!vecLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
        return;
    }

    // QGIS3ע�᷽ʽ
    QgsProject::instance()->addMapLayer(vecLayer);
    mapCanvas->setExtent(vecLayer->extent());
    layers.append(vecLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// �ۺ�������ʸ��ͼ�㱣�浽����
void DemoLabel::saveVectorLayerActionTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    QgsVectorLayer* saveLayer = (QgsVectorLayer*)layerTreeView->selectedLayers()[0];
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save shape file"), "", "*.shp");
    QgsVectorFileWriter::SaveVectorOptions options;
    options.driverName = QLatin1String("ESRI Shapefile");
    QgsVectorFileWriterTask* writerTask = new QgsVectorFileWriterTask(saveLayer, fileName, options);
    QgsApplication::taskManager()->addTask(writerTask);
}

// �ۺ�����������ʱ��ͼ��
void DemoLabel::addPointLayerActionTrigger()
{
    QString name = "point layer";

    QgsFields fields = QgsFields();
    QgsField fieldId = QgsField("id", QVariant::Int, "integer", 10, 0);
    QgsField fieldName = QgsField("name", QVariant::String, "string", 30, 0);
    QgsField fieldType = QgsField("type", QVariant::String, "string", 30, 0);
    fields.append(fieldId);
    fields.append(fieldName);
    fields.append(fieldType);

    QgsWkbTypes::Type geometrytype = QgsWkbTypes::Point;
    // ��������,"Point", "LineString", "Polygon", "MultiPoint", "MultiLineString", "MultiPolygon"    
    QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem("EPSG:4326");
    QgsVectorLayer* vLayer = QgsMemoryProviderUtils::createMemoryLayer(name, fields, geometrytype, crs);
    //QgsOgrProviderUtils::createEmptyDataSource(fileName, fileformat, enc, geometrytype, attributes, srs, errorMessage);

    // ��ͼ������ӵ�
    QgsVectorDataProvider* dataProvider = vLayer->dataProvider();
    //����һ��Ҫ��
    QgsFeature feature;
    //�����ڵ�ͼ�Ĳ���������
    feature.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(107.421, 33.9727)));
    feature.setAttributes(QgsAttributes() << QVariant(1) << QVariant("city") << QVariant("city"));
    dataProvider->addFeatures(QgsFeatureList() << feature);
    vLayer->updateExtents();

    // ��ͼ����ӵ�����
    QgsProject::instance()->addMapLayer(vLayer);
    //mapCanvas->setExtent(vLayer->extent());
    layers.append(vLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// ������ʱ��ͼ��
void DemoLabel::addLineLayerActionTrigger()
{
    QString name = "line layer";
    QgsFields fields = QgsFields();
    QgsField fieldId = QgsField("id", QVariant::Int, "integer", 10, 0);
    QgsField fieldName = QgsField("name", QVariant::String, "string", 30, 0);
    QgsField fieldType = QgsField("type", QVariant::String, "string", 30, 0);

    fields.append(fieldId);
    fields.append(fieldName);
    fields.append(fieldType);

    QgsWkbTypes::Type geometrytype = QgsWkbTypes::LineString;
    QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem("EPSG:4326");
    QgsVectorLayer* vLayer = QgsMemoryProviderUtils::createMemoryLayer(name, fields, geometrytype, crs);

    // ��ͼ���������
    QgsVectorDataProvider* dataProvider = vLayer->dataProvider();
    QgsFeature feature;
    QgsPolylineXY line;
    line << QgsPoint(107.594, 33.9226) << QgsPoint(107.61, 33.9179) << QgsPoint(107.611, 33.914);
    QgsGeometry geometry = QgsGeometry::fromPolylineXY(line);
    feature.setGeometry(geometry);
    feature.setAttributes(QgsAttributes() << QVariant(1) << QVariant("river") << QVariant("river"));
    dataProvider->addFeatures(QgsFeatureList() << feature);
    vLayer->updateExtents();

    // ��ͼ����ӵ�����
    QgsProject::instance()->addMapLayer(vLayer);
    //mapCanvas->setExtent(vLayer->extent());
    layers.append(vLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// ������ʱ�����ͼ��
void DemoLabel::addPolygonLayerActionTrigger()
{
    QString name = "polygon layer";
    QgsFields fields = QgsFields();
    QgsField fieldId = QgsField("id", QVariant::Int, "integer", 10, 0);
    QgsField fieldName = QgsField("name", QVariant::String, "string", 30, 0);
    QgsField fieldType = QgsField("type", QVariant::String, "string", 30, 0);

    fields.append(fieldId);
    fields.append(fieldName);
    fields.append(fieldType);

    QgsWkbTypes::Type geometrytype = QgsWkbTypes::Polygon;
    QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem("EPSG:4326");
    QgsVectorLayer* vLayer = QgsMemoryProviderUtils::createMemoryLayer(name, fields, geometrytype, crs);
    //QgsOgrProviderUtils::createEmptyDataSource(fileName, fileformat, enc, geometrytype, attributes, srs, errorMessage);

    // ��ͼ������Ӷ����
    QgsVectorDataProvider* dataProvider = vLayer->dataProvider();
    QgsFeature feature;
    QgsPolylineXY line;
    QgsPointXY p1, p2, p3;
    p1 = QgsPoint(107.755, 34.0624);
    p2 = QgsPoint(107.75, 34.0313);
    p3 = QgsPoint(107.783, 34.0621);
    line << p1 << p2 << p3 << p1;
    QgsPolygonXY polygon;
    polygon << line;
    polygon.fill(line);
    QgsGeometry geometry = QgsGeometry::fromPolygonXY(polygon);
    feature.setGeometry(geometry);
    feature.setAttributes(QgsAttributes() << QVariant(1) << QVariant("ocean") << QVariant("ocean"));
    dataProvider->addFeatures(QgsFeatureList() << feature);
    vLayer->updateExtents();

    // ��ͼ����ӵ�����
    QgsProject::instance()->addMapLayer(vLayer);
    //mapCanvas->setExtent(vLayer->extent());
    layers.append(vLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// ������ӵ�Ҫ�صı༭ģʽ
void DemoLabel::addPointTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    if (editFlag)
    {
        QMessageBox::information(this, "Failure", "Already in edit mode.");
        return;
    }
    editLayer = layerTreeView->selectedLayers()[0];
    editMapCamvas->setExtent(mapCanvas->extent());
    editMapCamvas->setLayers(layers);
    editMapCamvas->refresh();
    mapCanvas->stackUnder(editMapCamvas);
    editFlag = true;
    this->connect(editMapCamvas, SIGNAL(xyRelease(QgsPointXY)), this, SLOT(addPointFeatureTrigger(QgsPointXY)));
}

// ���������Ҫ�صı༭ģʽ
void DemoLabel::addLineTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    if (editFlag)
    {
        QMessageBox::information(this, "Failure", "Already in edit mode.");
        return;
    }
    editLayer = layerTreeView->selectedLayers()[0];
    editMapCamvas->setExtent(mapCanvas->extent());
    editMapCamvas->setLayers(layers);
    editMapCamvas->refresh();
    mapCanvas->stackUnder(editMapCamvas);
    editFlag = true;
    addLineFlag = false;
    addedHighLightFlag = false;
    addingHighLightFlag = false;
    this->connect(editMapCamvas, SIGNAL(xyRelease(QgsPointXY)), this, SLOT(addLinePointTrigger(QgsPointXY)));
    this->connect(editMapCamvas, SIGNAL(xyReleaseRight(QgsPointXY)), this, SLOT(addLineFeatureTrigger()));
}

// ��������Ҫ�صı༭ģʽ
void DemoLabel::addPolygonTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    if (editFlag)
    {
        QMessageBox::information(this, "Failure", "Already in edit mode.");
        return;
    }
    editLayer = layerTreeView->selectedLayers()[0];
    editMapCamvas->setExtent(mapCanvas->extent());
    editMapCamvas->setLayers(layers);
    editMapCamvas->refresh();
    mapCanvas->stackUnder(editMapCamvas);
    editFlag = true;
    addPolygonFlag = false;
    addedHighLightFlag = false;
    addingHighLightFlag = false;
    this->connect(editMapCamvas, SIGNAL(xyRelease(QgsPointXY)), this, SLOT(addPolygonPointTrigger(QgsPointXY)));
    this->connect(editMapCamvas, SIGNAL(xyReleaseRight(QgsPointXY)), this, SLOT(addPolygonFeatureTrigger()));
}

// �����༭ģʽ
void DemoLabel::saveEditTrigger()
{
    if (!editFlag) {
        QMessageBox::information(this, "Failure", "Not in edit mode.");
        return;
    }
    editMapCamvas->stackUnder(mapCanvas);
    this->disconnect(editMapCamvas, SIGNAL(xyRelease(QgsPointXY)), this, SLOT(addPointFeatureTrigger(QgsPointXY)));
    this->disconnect(editMapCamvas, SIGNAL(xyRelease(QgsPointXY)), this, SLOT(addLinePointTrigger(QgsPointXY)));
    this->disconnect(editMapCamvas, SIGNAL(xyReleaseRight(QgsPointXY)), this, SLOT(addLineFeatureTrigger()));
    this->disconnect(editMapCamvas, SIGNAL(xyRelease(QgsPointXY)), this, SLOT(addPolygonPointTrigger(QgsPointXY)));
    this->disconnect(editMapCamvas, SIGNAL(xyReleaseRight(QgsPointXY)), this, SLOT(addPolygonFeatureTrigger()));
    //addLine.clear();
    //addPolygon.clear();
    addLineFlag = false;
    addPolygonFlag = false;
    editFlag = false;
}

// �༭����ʽ
void DemoLabel::editPointStyleTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    editLayer = layerTreeView->selectedLayers()[0];
    EditFeatureStyleBox* pointStyleBox = new EditFeatureStyleBox((QgsVectorLayer*)editLayer);
    pointStyleBox->setWindowModality(Qt::ApplicationModal);
    pointStyleBox->show();
    connect(pointStyleBox, &EditFeatureStyleBox::editFeatureStyleConfirm, this, [=]
        {
            mapCanvas->refresh();
        });
}

// �༭����ʽ
void DemoLabel::editLineStyleTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    editLayer = layerTreeView->selectedLayers()[0];
    EditFeatureStyleBox* lineStyleBox = new EditFeatureStyleBox((QgsVectorLayer*)editLayer);
    lineStyleBox->setWindowModality(Qt::ApplicationModal);
    lineStyleBox->show();
    connect(lineStyleBox, &EditFeatureStyleBox::editFeatureStyleConfirm, this, [=]
        {
            mapCanvas->refresh();
        });
}

// �༭�������ʽ
void DemoLabel::editPolygonStyleTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    editLayer = layerTreeView->selectedLayers()[0];
    EditFeatureStyleBox* polygonStyleBox = new EditFeatureStyleBox((QgsVectorLayer*)editLayer);
    polygonStyleBox->setWindowModality(Qt::ApplicationModal);
    polygonStyleBox->show();
    connect(polygonStyleBox, &EditFeatureStyleBox::editFeatureStyleConfirm, this, [=]
        {
            mapCanvas->refresh();
        });
}

// ��ʾ��ǩ
void DemoLabel::showLabelTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    labelLayer = layerTreeView->selectedLayers()[0];
    QgsVectorLayer* mLayer = (QgsVectorLayer*)labelLayer;

    // �����Ƕ���һ�� QgsPalLayerSettings ������������������������
    QgsPalLayerSettings layerSettings;
    layerSettings.drawLabels = true;
    //����API�ĵ��е����ԣ������Զ�������
    layerSettings.fieldName = mLayer->fields()[1].name(); // ����Labelͼ�����ע�ֶ�
    //layerSettings.fieldName = mLayer->fields().field("name").name();
    layerSettings.centroidWhole = true; // ����λ�òο������ĵ㣨��������������or�����ɼ����֣�
    layerSettings.isExpression = true;  // ��ǩ�ɱ��ʽ�ַ������
    if (mLayer->geometryType() == QgsWkbTypes::PointGeometry)
    {
        layerSettings.placement = QgsPalLayerSettings::AroundPoint;   // ��ѡ��ǩ����ģʽ
    }
    else if (mLayer->geometryType() == QgsWkbTypes::LineGeometry)
    {
        layerSettings.placement = QgsPalLayerSettings::Line;   // ��ѡ��ǩ����ģʽ
    }
    else if (mLayer->geometryType() == QgsWkbTypes::PolygonGeometry)
    {
        layerSettings.placement = QgsPalLayerSettings::Horizontal;   // ��ѡ��ǩ����ģʽ
    }
    else
    {
        return;
    }
    layerSettings.yOffset = 2.5;    // ��ǩ��ֱƫ��
    // Label ��������:����buffer����
    QgsTextBufferSettings buffersettings;
    buffersettings.setEnabled(true);
    buffersettings.setSize(1);
    buffersettings.setColor(QColor(255, 255, 0));
    //������Ӱ
    QgsTextShadowSettings shadowsetings;
    shadowsetings.setEnabled(true);
    shadowsetings.setOffsetAngle(135);
    shadowsetings.setOffsetDistance(1);
    //�����ʽ
    QgsTextFormat textformat;
    QFont font("SimSun", 5, 5, false);
    font.setUnderline(false);
    textformat.setFont(font);
    textformat.setBuffer(buffersettings);
    textformat.setShadow(shadowsetings);
    layerSettings.setFormat(textformat);
    QgsVectorLayerSimpleLabeling* labeling = new QgsVectorLayerSimpleLabeling(layerSettings);
    mLayer->setLabeling(labeling);
    mLayer->setLabelsEnabled(true);
    mapCanvas->refresh();
}

// ���ر�ǩ
void DemoLabel::hiddenLabelTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    labelLayer = layerTreeView->selectedLayers()[0];
    QgsVectorLayer* mLayer = (QgsVectorLayer*)labelLayer;
    mLayer->setLabelsEnabled(false);
    mapCanvas->refresh();
}

// ������Ϣʶ��ģʽ
void DemoLabel::identifyStratTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    infoLayer = layerTreeView->selectedLayers()[0];
    QgsVectorLayer* vLayer = (QgsVectorLayer*)infoLayer;
    identifyTool = new QgsMapToolIdentifyFeature(mapCanvas, vLayer);
    this->connect(identifyTool, SIGNAL(featureIdentified(QgsFeature)), this, SLOT(showInfoTrigger(QgsFeature)));
    mapCanvas->setMapTool(identifyTool);
    infoFlag = true;
    infoHighLightFlag = false;
}

// �˳���Ϣʶ��ģʽ
void DemoLabel::identifyEndTrigger()
{
    if (!infoFlag)
    {
        QMessageBox::information(this, "Failure", "Not in information identify mode.");
        return;
    }
    mapCanvas->unsetMapTool(identifyTool);
    infoTable->clearContents();
    infoTable->setRowCount(0);
    infoFlag = false;
    if (infoHighLightFlag)
    {
        highLight->hide();
        infoHighLightFlag = false;
    }
}

// ��ʾҪ����Ϣ
void DemoLabel::showInfoTrigger(QgsFeature f)
{
    //QgsVectorLayer* vlayer = (QgsVectorLayer*)infoLayer;
    //QgsFeature f =  vlayer->getFeature(ff);
    QgsFields field = f.fields(); // ����Ҫ������
    QgsAttributes attribute = f.attributes(); // ����Ҫ�����Ա���
    //QMessageBox::information(this, "", QString(field.count()));
    infoTable->setRowCount(field.count());
    //f.setAttribute("id", 101);
    //vlayer->startEditing();
    //vlayer->changeAttributeValue(ff, f.fields().indexOf("id"), 101);
    //vlayer->commitChanges();
    for (int i = 0; i < field.count(); i++)
    {
        infoTable->setItem(i, 0, new QTableWidgetItem(field.at(i).name()));
        infoTable->setItem(i, 1, new QTableWidgetItem(attribute.at(i).toString()));
    }

    if (infoHighLightFlag)
    {
        highLight->hide();
    }
    QgsVectorLayer* vecLayer = (QgsVectorLayer*)infoLayer;
    highLight = new QgsHighlight(mapCanvas, f, vecLayer);
    highLight->setColor(Qt::red);
    highLight->show();
    infoHighLightFlag = true;
}

// ��״̬������ʾ�������
void DemoLabel::showMousePointTrigger(const QgsPointXY& p)
{
    lblCoordinate->setText(QString("x=%1, y=%2").arg(p.x()).arg(p.y()));
}

// �򿪵�Ҫ���������봰��
void DemoLabel::addPointFeatureTrigger(const QgsPointXY& p)
{
    // ������
    QgsFeature myFeature;
    QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
    QgsVectorDataProvider* dataProvider = mVecLayer->dataProvider();
    //QMessageBox::information(this, "",QString::number(dataProvider->featureCount()));
    addPoint = p;
    QMap<QString, QVariant> msettings;
    for (int i = 0; i < dataProvider->fields().size(); i++)
    {
        if (i == dataProvider->fieldNameIndex("id"))
        {
            //QMessageBox::information(this, "", dataProvider->fields()[i].name());
            //msettings.insert("id", 0);
            msettings.insert(dataProvider->fields()[i].name(), QVariant(dataProvider->featureCount()));
        }
        else
        {
            msettings.insert(dataProvider->fields()[i].name(), QVariant());
        }
    }
    AddFeatureSettingBox* mAFSB = new AddFeatureSettingBox(msettings);
    mAFSB->setPoint(p);
    mAFSB->setWindowModality(Qt::ApplicationModal);
    mAFSB->show();
    this->connect(mAFSB, SIGNAL(settingConfirm(QMap<QString, QVariant>)), this, SLOT(addPointFeatureConfirmTrigger(QMap<QString, QVariant>)));
}

// ��ӵ�Ҫ�ص�ʸ��ͼ��
void DemoLabel::addPointFeatureConfirmTrigger(QMap<QString, QVariant> settings)
{
    //QMessageBox::information(this, "", settings["name"].toString());
    // ������
    QgsFeature myFeature;
    QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
    QgsVectorDataProvider* dataProvider = mVecLayer->dataProvider();
    myFeature.setGeometry(QgsGeometry::fromPointXY(addPoint));
    //int id = qrand() % 100;
    //QString name = QString(names[qrand() % 26]) + QString(names[qrand() % 26]) + QString(names[qrand() % 26]) + QString(names[qrand() % 26]) + QString(names[qrand() % 26]);
    //QString type = pointType[qrand() % pointType.count()];
    myFeature.setAttributes(QgsAttributes() << settings["id"] << settings["name"] << settings["type"]);

    //QMessageBox::information(this, "", QString::number(myFeature.attributes().size()));
    //QMessageBox::information(this, "", QString::number(settings.size()));
    // ��ʼ�༭
    mVecLayer->startEditing();

    // ���Ҫ��myFeature1
    dataProvider->addFeatures(QgsFeatureList() << myFeature);
    mVecLayer->commitChanges();
    mVecLayer->updateExtents();
    mapCanvas->refresh();
}

// ��Ҫ��ȡ��
void DemoLabel::addLinePointTrigger(const QgsPointXY& p)
{
    if (!addLineFlag)
    {
        addLine.clear();
        addLineFlag = true;
    }
    addLine.append(p);
    if (addLine.size() == 1)
    {
        this->connect(editMapCamvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showLineTrigger(QgsPointXY)));
    }
    if (addLine.size() > 1)
    {
        QgsFeature myFeature = getAddLineFeature();
        QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
        if (addedHighLightFlag)
        {
            addedHighLight->hide();
        }
        addedHighLight = new QgsHighlight(editMapCamvas, myFeature, mVecLayer);
        addedHighLight->setColor(Qt::red);
        addedHighLight->show();
        addedHighLightFlag = true;
    }
}

// ����Ҫ���������봰��
void DemoLabel::addLineFeatureTrigger()
{
    addLineFlag = false;
    this->disconnect(editMapCamvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showLineTrigger(QgsPointXY)));
    if (addLine.size() < 2)
    {
        QMessageBox::information(this, "Failure", "No line.");
        return;
    }
    // ������
    //QMessageBox::information(this, "", "line");
    QgsFeature myFeature;
    QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
    QgsVectorDataProvider* dataProvider = mVecLayer->dataProvider();

    QMap<QString, QVariant> msettings;
    for (int i = 0; i < dataProvider->fields().size(); i++)
    {
        if (i == dataProvider->fieldNameIndex("id"))
        {
            //QMessageBox::information(this, "", dataProvider->fields()[i].name());
            //msettings.insert("id", 0);
            msettings.insert(dataProvider->fields()[i].name(), QVariant(dataProvider->featureCount()));
        }
        else
        {
            msettings.insert(dataProvider->fields()[i].name(), QVariant());
        }
    }
    AddFeatureSettingBox* mAFSB = new AddFeatureSettingBox(msettings);
    mAFSB->setWindowModality(Qt::ApplicationModal);
    mAFSB->show();
    this->connect(mAFSB, SIGNAL(settingConfirm(QMap<QString, QVariant>)), this, SLOT(addLineFeatureConfirmTrigger(QMap<QString, QVariant>)));
    this->connect(mAFSB, &AddFeatureSettingBox::settingCancel, this, [=]
        {
            if (addedHighLightFlag)
            {
                addedHighLight->hide();
                addedHighLightFlag = false;
            }
        });
    if (addingHighLightFlag)
    {
        addingHighLight->hide();
        addingHighLightFlag = false;
    }
}

// �����Ҫ�ص�ʸ��ͼ��
void DemoLabel::addLineFeatureConfirmTrigger(QMap<QString, QVariant> settings)
{
    // ������
    //QMessageBox::information(this, "", "line");
    QgsFeature myFeature;
    QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
    QgsVectorDataProvider* dataProvider = mVecLayer->dataProvider();
    QgsPolylineXY line;
    for (int i = 0; i < addLine.size(); i++)
    {
        line << addLine[i];
    }
    QgsGeometry geometry = QgsGeometry::fromPolylineXY(line);
    myFeature.setGeometry(geometry);
    myFeature.setAttributes(QgsAttributes() << settings["id"] << settings["name"] << settings["type"]);

    // ��ʼ�༭
    mVecLayer->startEditing();

    // ���Ҫ��myFeature1
    dataProvider->addFeatures(QgsFeatureList() << myFeature);
    mVecLayer->commitChanges();
    mVecLayer->updateExtents();
    mapCanvas->refresh();
    addLine.clear();

    if (addedHighLightFlag)
    {
        addedHighLight->hide();
        addedHighLightFlag = false;
    }
}

// ��ʾ����Ƶ���Ҫ��
void DemoLabel::showLineTrigger(const QgsPointXY& p)
{
    QgsPointXY startPoint = addLine.back();
    if (addLine.size() > 0)
    {
        QgsFeature myFeature;
        QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
        QgsPolylineXY line;
        line << startPoint << p;
        QgsGeometry geometry = QgsGeometry::fromPolylineXY(line);
        myFeature.setGeometry(geometry);
        if (addingHighLightFlag)
        {
            addingHighLight->hide();
        }
        addingHighLight = new QgsHighlight(editMapCamvas, myFeature, mVecLayer);
        addingHighLight->setColor(Qt::green);
        addingHighLight->show();
        addingHighLightFlag = true;
    }
}

// �����Ҫ��ȡ��
void DemoLabel::addPolygonPointTrigger(const QgsPointXY& p)
{
    if (!addPolygonFlag)
    {
        addPolygon.clear();
        addPolygonFlag = true;
    }
    addPolygon.append(p);
    if (addPolygon.size() == 1)
    {
        this->connect(editMapCamvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showPolygonTrigger(QgsPointXY)));
    }
    if (addPolygon.size() > 1)
    {
        QgsFeature myFeature = getAddPolygonFeature();
        QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
        if (addedHighLightFlag)
        {
            addedHighLight->hide();
        }
        addedHighLight = new QgsHighlight(editMapCamvas, myFeature, mVecLayer);
        addedHighLight->setColor(Qt::red);
        addedHighLight->show();
        addedHighLightFlag = true;
    }
}

// �򿪶����Ҫ������ҳ��
void DemoLabel::addPolygonFeatureTrigger()
{
    addPolygonFlag = false;
    this->disconnect(editMapCamvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showPolygonTrigger(QgsPointXY)));
    if (addPolygon.size() < 3)
    {
        QMessageBox::information(this, "Failure", "No polygon.");
        return;
    }
    // ���������
    QgsFeature myFeature;
    QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
    QgsVectorDataProvider* dataProvider = mVecLayer->dataProvider();

    QMap<QString, QVariant> msettings;
    for (int i = 0; i < dataProvider->fields().size(); i++)
    {
        if (i == dataProvider->fieldNameIndex("id"))
        {
            msettings.insert(dataProvider->fields()[i].name(), QVariant(dataProvider->featureCount()));
        }
        else
        {
            msettings.insert(dataProvider->fields()[i].name(), QVariant());
        }
    }
    AddFeatureSettingBox* mAFSB = new AddFeatureSettingBox(msettings);
    mAFSB->setWindowModality(Qt::ApplicationModal);
    mAFSB->show();
    this->connect(mAFSB, SIGNAL(settingConfirm(QMap<QString, QVariant>)), this, SLOT(addPolygonFeatureConfirmTrigger(QMap<QString, QVariant>)));
    this->connect(mAFSB, &AddFeatureSettingBox::settingCancel, this, [=]
        {
            if (addedHighLightFlag)
            {
                addedHighLight->hide();
                addedHighLightFlag = false;
            }
        });
    if (addingHighLightFlag)
    {
        addingHighLight->hide();
        addingHighLightFlag = false;
    }
}

// ��Ӷ����Ҫ�ص�ʸ��ͼ��
void DemoLabel::addPolygonFeatureConfirmTrigger(QMap<QString, QVariant> settings)
{
    // ���������
    QgsFeature myFeature;
    QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
    QgsVectorDataProvider* dataProvider = mVecLayer->dataProvider();
    QgsPolygonXY polygon;
    QgsPolylineXY line;
    for (int i = 0; i < addPolygon.length(); i++)
    {
        line << addPolygon[i];
    }
    line << addPolygon[0];
    polygon.append(line);
    polygon.fill(line);
    QgsGeometry geometry = QgsGeometry::fromPolygonXY(polygon);
    myFeature.setGeometry(geometry);
    myFeature.setAttributes(QgsAttributes() << settings["id"] << settings["name"] << settings["type"]);

    // ��ʼ�༭
    mVecLayer->startEditing();

    // ���Ҫ��myFeature1
    dataProvider->addFeatures(QgsFeatureList() << myFeature);
    mVecLayer->commitChanges();
    mVecLayer->updateExtents();
    mapCanvas->refresh();
    addLine.clear();

    if (addedHighLightFlag)
    {
        addedHighLight->hide();
        addedHighLightFlag = false;
    }
}

// ��ʾ����ƵĶ����
void DemoLabel::showPolygonTrigger(const QgsPointXY& p)
{
    QgsPointXY startPoint = addPolygon.back();
    if (addPolygon.size() > 0)
    {
        QgsFeature myFeature;
        QgsVectorLayer* mVecLayer = (QgsVectorLayer*)editLayer;
        QgsPolygonXY polygon;
        QgsPolylineXY line;
        line << addPolygon.first() << addPolygon.back() << p;
        polygon.append(line);
        polygon.fill(line);
        QgsGeometry geometry = QgsGeometry::fromPolygonXY(polygon);
        myFeature.setGeometry(geometry);
        if (addingHighLightFlag)
        {
            addingHighLight->hide();
        }
        addingHighLight = new QgsHighlight(editMapCamvas, myFeature, mVecLayer);
        addingHighLight->setColor(Qt::green);
        addingHighLight->show();
        addingHighLightFlag = true;
    }
}

// �Ե�ǰѡ��ͼ�㷴��Ȩ��ֵ
void DemoLabel::idwInterpolateTrigger()
{
    QgsVectorLayer* vecLayer = (QgsVectorLayer*)(layerTreeView->selectedLayers()[0]);
    if (vecLayer->isValid() && vecLayer->geometryType() == QgsWkbTypes::GeometryType::PointGeometry)
    {
        QgsRasterLayer* rasLayer = achieveIDWInterpolate(vecLayer);

        if (rasLayer == nullptr)
        {
            QMessageBox::information(this, "Failure", "No layer.");
            return;
        }

        // ���դ������
        QgsProject::instance()->addMapLayer(rasLayer);   // ע��
        mapCanvas->setExtent(rasLayer->extent());        // ��������Χ����Ϊդ��ͼ�㷶Χ
        layers.append(rasLayer);                         // ��դ��ͼ��׷�ӵ�������
        mapCanvas->setLayers(layers);                       // ��ͼ�㻭��������
        mapCanvas->setVisible(true);
        mapCanvas->freeze(false);
        mapCanvas->refresh();                               // ���»���
    }
    else
    {
        QMessageBox::information(this, "Failure", "Cannot interpolate.");
        return;
    }
}

// �Ե�ǰѡ��ͼ�����ǲ�ֵ
void DemoLabel::tinInterpolateTrigger()
{
}

// ui����
void DemoLabel::InitUI()
{
    InitMenuBar();
    InitToolBar();
    InitMapCanvas();
    InitInfoTable();
    InitStatusBar();
    // դ�񲼾�
    QWidget* centralWidget = this->centralWidget();
    QGridLayout* centralLayout = new QGridLayout(centralWidget);
    centralLayout->addWidget(layerTreeView, 0, 0, 2, 1);            // ͼ�������λ��
    centralLayout->addWidget(mapCanvas, 0, 1, 2, 1);                // ����λ�� ��������ʼ���У�ռλ����
    centralLayout->addWidget(editMapCamvas, 0, 1, 2, 1);
    centralLayout->addLayout(infoAreaLayout, 0, 2, 2, 2);
    editMapCamvas->stackUnder(mapCanvas);
	//�������
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowTitle(QString::fromLocal8Bit("DemoLabel"));

    // ��ʼ��Ҫ������
    QTime randTime = QTime::currentTime();
    qsrand(randTime.msec() + randTime.second() * 1000);
    names = "qwertyuiopasdfghjklzxcvbnm";
    pointType = QList<QString>() << "city" << "peak" << "landmark";
    lineType = QList<QString>() << "river" << "road" << "pipeline";
    polygonType = QList<QString>() << "ocean" << "island" << "lake";
}

// �˵�����ʼ��
void DemoLabel::InitMenuBar()
{
    fileMenu = this->menuBar()->addMenu("File");

    openRasterFileAction = new QAction("Open raster", this);
    this->connect(openRasterFileAction, SIGNAL(triggered(bool)), this, SLOT(openRasterFileActionTrigger()));
    fileMenu->addAction(openRasterFileAction);

    openVectorFileAction = new QAction("Open vector", this);
    this->connect(openVectorFileAction, SIGNAL(triggered(bool)), this, SLOT(openVectorFileActionTrigger()));
    fileMenu->addAction(openVectorFileAction);

    saveVectorLayerAction = new QAction("Save vector", this);
    this->connect(saveVectorLayerAction, SIGNAL(triggered(bool)), this, SLOT(saveVectorLayerActionTrigger()));
    fileMenu->addAction(saveVectorLayerAction);

    editMenu = this->menuBar()->addMenu("Edit");

    addPointLayerAction = new QAction("Point layer", this);
    this->connect(addPointLayerAction, SIGNAL(triggered(bool)), this, SLOT(addPointLayerActionTrigger()));
    editMenu->addAction(addPointLayerAction);

    addLineLayerAction = new QAction("Line layer", this);
    this->connect(addLineLayerAction, SIGNAL(triggered(bool)), this, SLOT(addLineLayerActionTrigger()));
    editMenu->addAction(addLineLayerAction);

    addPolygonLayerAction = new QAction("Polygon layer", this);
    this->connect(addPolygonLayerAction, SIGNAL(triggered(bool)), this, SLOT(addPolygonLayerActionTrigger()));
    editMenu->addAction(addPolygonLayerAction);

    processMenu = this->menuBar()->addMenu("Process");

    idwInterpolateAction = new QAction("Interpolate(idw)", this);
    this->connect(idwInterpolateAction, SIGNAL(triggered(bool)), this, SLOT(idwInterpolateTrigger()));
    processMenu->addAction(idwInterpolateAction);

    tinInterpolateAction = new QAction("Interpolate(tin)", this);
    this->connect(tinInterpolateAction, SIGNAL(triggered(bool)), this, SLOT(tinInterpolateTrigger()));
    processMenu->addAction(tinInterpolateAction);
}

// ��������ʼ��
void DemoLabel::InitToolBar()
{
    editFlag = false;
    infoFlag = false;

    btnAddPoint = new QPushButton();
    btnAddPoint->setIcon(QIcon("addPoint.png"));
    this->connect(btnAddPoint, SIGNAL(clicked(bool)), this, SLOT(addPointTrigger()));
    ui.mainToolBar->addWidget(btnAddPoint);

    btnAddLine = new QPushButton();
    btnAddLine->setIcon(QIcon("addLine.png"));
    this->connect(btnAddLine, SIGNAL(clicked(bool)), this, SLOT(addLineTrigger()));
    ui.mainToolBar->addWidget(btnAddLine);

    btnAddPolygon = new QPushButton();
    btnAddPolygon->setIcon(QIcon("addPolygon.png"));
    this->connect(btnAddPolygon, SIGNAL(clicked(bool)), this, SLOT(addPolygonTrigger()));
    ui.mainToolBar->addWidget(btnAddPolygon);

    btnSaveEdit = new QPushButton();
    btnSaveEdit->setIcon(QIcon("addSave.png"));
    this->connect(btnSaveEdit, SIGNAL(clicked(bool)), this, SLOT(saveEditTrigger()));
    ui.mainToolBar->addWidget(btnSaveEdit);

    ui.mainToolBar->addSeparator();

    btnEditPointStyle = new QPushButton();
    btnEditPointStyle->setIcon(QIcon("editPoint.png"));
    this->connect(btnEditPointStyle, SIGNAL(clicked(bool)), this, SLOT(editPointStyleTrigger()));
    ui.mainToolBar->addWidget(btnEditPointStyle);

    btnEditLineStyle = new QPushButton();
    btnEditLineStyle->setIcon(QIcon("editLine.png"));
    this->connect(btnEditLineStyle, SIGNAL(clicked(bool)), this, SLOT(editLineStyleTrigger()));
    ui.mainToolBar->addWidget(btnEditLineStyle);

    btnEditPolygonStyle = new QPushButton();
    btnEditPolygonStyle->setIcon(QIcon("editPolygon.png"));
    this->connect(btnEditPolygonStyle, SIGNAL(clicked(bool)), this, SLOT(editPolygonStyleTrigger()));
    ui.mainToolBar->addWidget(btnEditPolygonStyle);

    btnShowLabel = new QPushButton();
    btnShowLabel->setIcon(QIcon("showLabel.png"));
    this->connect(btnShowLabel, SIGNAL(clicked(bool)), this, SLOT(showLabelTrigger()));
    ui.mainToolBar->addWidget(btnShowLabel);

    btnHiddenLabel = new QPushButton();
    btnHiddenLabel->setIcon(QIcon("hideLabel.png"));
    this->connect(btnHiddenLabel, SIGNAL(clicked(bool)), this, SLOT(hiddenLabelTrigger()));
    ui.mainToolBar->addWidget(btnHiddenLabel);

    ui.mainToolBar->addSeparator();

    btnIdentifyOn = new QPushButton();
    btnIdentifyOn->setIcon(QIcon("info.png"));
    this->connect(btnIdentifyOn, SIGNAL(clicked(bool)), this, SLOT(identifyStratTrigger()));
    ui.mainToolBar->addWidget(btnIdentifyOn);

    btnIdentifyOff = new QPushButton();
    btnIdentifyOff->setIcon(QIcon("info-exit.png"));
    this->connect(btnIdentifyOff, SIGNAL(clicked(bool)), this, SLOT(identifyEndTrigger()));
    ui.mainToolBar->addWidget(btnIdentifyOff);
}

// ��ʼ����ͼ����
void DemoLabel::InitMapCanvas()
{
    mapCanvas = new QgsMapCanvas();
    //this->setCentralWidget(mapCanvas);                  // ����ͼ��������������������
    mapCanvas->setCanvasColor(QColor(255, 255, 255));   // ���õ�ͼ����Ϊ��ɫ
    mapCanvas->setVisible(true);
    mapCanvas->enableAntiAliasing(true);

    editMapCamvas = new QgsEditMapCanvas();
    editMapCamvas->setCanvasColor(QColor(0, 0, 0, 0));
    editMapCamvas->setVisible(true);
    editMapCamvas->enableAntiAliasing(true);

    //-----------------------------------

    // ��ʼ��ͼ�������
    layerTreeView = new QgsLayerTreeView(this);
    initLayerTreeView();
}

// ��ʼ��ͼ�������
void DemoLabel::initLayerTreeView()
{
    QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
    model->setFlag(QgsLayerTreeModel::AllowNodeRename);
    model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
    model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
    model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
    model->setAutoCollapseLegendNodes(10);
    layerTreeView->setModel(model);
    layerTreeView->setFixedWidth(200);

    // ���ӵ�ͼ������ͼ�������
    layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), mapCanvas, this);
    connect(QgsProject::instance(), SIGNAL(writeProject(QDomDocument&)), layerTreeCanvasBridge, SLOT(writeProject(QDomDocument&)));
    connect(QgsProject::instance(), SIGNAL(readProject(QDomDocument&)), layerTreeCanvasBridge, SLOT(readProject(QDomDocument&)));
}

// �������Ա�
void DemoLabel::InitInfoTable()
{
    infoAreaLayout = new QVBoxLayout();
    infoTable = new QTableWidget(0, 2);
    infoTable->setHorizontalHeaderLabels(QStringList() << "Attribute" << "Value");
    //infoTable->setFixedWidth(300);
    infoAreaLayout->addWidget(infoTable);
}

// ������ʾ
void DemoLabel::InitStatusBar()
{
    lblCoordinate = new QLabel;
    lblCoordinate->setMinimumSize(100, 20);
    lblCoordinate->setFrameShape(QFrame::WinPanel);
    lblCoordinate->setFrameShadow(QFrame::Sunken);
    ui.statusBar->addWidget(lblCoordinate);
    this->connect(mapCanvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showMousePointTrigger(QgsPointXY)));
}

// �ж�ѡ���ͼ���Ƿ�Ϊʸ��ͼ��
bool DemoLabel::judgeOneVectoerSelect(QList<QgsMapLayer*> selectLayers)
{
    if (selectLayers.count() > 1)
    {
        QMessageBox::information(this, "Failure", "Select more than one layer.");
        return false;
    }
    if (selectLayers.count() == 0)
    {
        QMessageBox::information(this, "Failure", "Select no layer.");
        return false;
    }
    if (selectLayers[0]->type() != QgsMapLayerType::VectorLayer)
    {
        QMessageBox::information(this, "Failure", "Select layer is not vector layer.");
        return false;
    }
    return true;
}

// ����������ӵ���Ҫ��
QgsFeature DemoLabel::getAddLineFeature()
{
    QgsFeature myFeature;
    QgsPolylineXY line;
    for (int i = 0; i < addLine.size(); i++)
    {
        line << addLine[i];
    }
    QgsGeometry geometry = QgsGeometry::fromPolylineXY(line);
    myFeature.setGeometry(geometry);
    return myFeature;
}

// ����������ӵĶ����Ҫ��
QgsFeature DemoLabel::getAddPolygonFeature()
{
    QgsFeature myFeature;
    QgsPolygonXY polygon;
    QgsPolylineXY line;
    for (int i = 0; i < addPolygon.length(); i++)
    {
        line << addPolygon[i];
    }
    line << addPolygon[0];
    polygon.append(line);
    polygon.fill(line);
    QgsGeometry geometry = QgsGeometry::fromPolygonXY(polygon);
    myFeature.setGeometry(geometry);
    return myFeature;
}

// ʵ�ַ���Ȩ��ֵ
QgsRasterLayer* DemoLabel::achieveIDWInterpolate(QgsVectorLayer* layer)
{
    struct QgsInterpolator::LayerData layerDate;
    layerDate.interpolationAttribute = 0;
    layerDate.source = layer;
    layerDate.sourceType = QgsInterpolator::SourceType::SourcePoints;
    layerDate.valueSource = QgsInterpolator::ValueSource::ValueAttribute;
    QList<QgsInterpolator::LayerData> inputLayers;
    inputLayers.append(layerDate);
    QgsIDWInterpolator* idw = new QgsIDWInterpolator(inputLayers);

    int rows = (layer->extent().xMaximum() - layer->extent().xMinimum()) / 0.1;
    int columns = (layer->extent().yMaximum() - layer->extent().yMinimum()) / 0.1;
    QString exeFilePath = QCoreApplication::applicationDirPath();
    QString dirName = exeFilePath + "/temp";
    QDir dir(dirName);
    if (!dir.exists())
    {
        dir.mkdir(dirName);
    }
    QString tempFileName = dirName + "/idwInterpolate.tif";
    QgsGridFileWriter* gridWriter = new QgsGridFileWriter(idw, tempFileName, layer->extent(), rows, columns);
    qint32 writeResult = gridWriter->writeFile(); // �ɹ�����0�� ʧ�ܷ���1
    if (writeResult != 0)
    {
        QMessageBox::information(this, "Failure", "Cannot write file.");
        return nullptr;
    }

    QgsRasterLayer* rasterLayer = new QgsRasterLayer(tempFileName, "idwInterpolate.tif", "gdal");
    if (!rasterLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + tempFileName);
        return nullptr;
    }
    return rasterLayer;
}

// ʵ�����ǲ�ֵ
QgsRasterLayer* DemoLabel::achieveTINInterpolate(QgsVectorLayer* layer)
{
    return nullptr;
}
