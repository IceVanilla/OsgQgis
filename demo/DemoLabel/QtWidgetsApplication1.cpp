#include "QtWidgetsApplication1.h"
#include <qmenubar.h>       // QT菜单栏库，存放菜单
#include <qmessagebox.h>    // QT信息盒子，显示操作提示
#include <qfiledialog.h>    // QT文件目录库，打开文件选择窗口
#include <qinputdialog.h>

#include <qgslayertreemodel.h>  // 使用给定树构建新的树模型，一般与QgsLayerTreeView一起使用
#include <qgridlayout.h>        // 栅格布局管理器

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

// 槽函数：打开并显示栅格数据
void DemoLabel::openRasterFileActionTrigger()
{
    // 打开文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster file"), "", "remote sensing image(*.tif *.tiff);;image(*.jpg *.jpeg *.png *.bmp)");
    if (fileName.isNull())
    {
        return;
    }
    QStringList temp = fileName.split('/');
    QString basename = temp.at(temp.size() - 1);

    // 创建QgsrRasterLayer类
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(fileName, basename, "gdal");
    if (!rasterLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
        return;
    }

    // 添加栅格数据
    QgsProject::instance()->addMapLayer(rasterLayer);   // 注册
    mapCanvas->setExtent(rasterLayer->extent());        // 将画布范围设置为栅格图层范围
    layers.append(rasterLayer);                         // 将栅格图层追加到链表中
    mapCanvas->setLayers(layers);                       // 将图层画到画布上
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();                               // 更新画布
}

// 槽函数：打开并显示矢量数据
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

    // QGIS3注册方式
    QgsProject::instance()->addMapLayer(vecLayer);
    mapCanvas->setExtent(vecLayer->extent());
    layers.append(vecLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// 槽函数：将矢量图层保存到本地
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

// 槽函数：创建临时点图层
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
    // 几何类型,"Point", "LineString", "Polygon", "MultiPoint", "MultiLineString", "MultiPolygon"    
    QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem("EPSG:4326");
    QgsVectorLayer* vLayer = QgsMemoryProviderUtils::createMemoryLayer(name, fields, geometrytype, crs);
    //QgsOgrProviderUtils::createEmptyDataSource(fileName, fileformat, enc, geometrytype, attributes, srs, errorMessage);

    // 在图层中添加点
    QgsVectorDataProvider* dataProvider = vLayer->dataProvider();
    //创建一个要素
    QgsFeature feature;
    //设置在地图的插入点的坐标
    feature.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(107.421, 33.9727)));
    feature.setAttributes(QgsAttributes() << QVariant(1) << QVariant("city") << QVariant("city"));
    dataProvider->addFeatures(QgsFeatureList() << feature);
    vLayer->updateExtents();

    // 将图层添加到画布
    QgsProject::instance()->addMapLayer(vLayer);
    //mapCanvas->setExtent(vLayer->extent());
    layers.append(vLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// 创建临时线图层
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

    // 在图层中添加线
    QgsVectorDataProvider* dataProvider = vLayer->dataProvider();
    QgsFeature feature;
    QgsPolylineXY line;
    line << QgsPoint(107.594, 33.9226) << QgsPoint(107.61, 33.9179) << QgsPoint(107.611, 33.914);
    QgsGeometry geometry = QgsGeometry::fromPolylineXY(line);
    feature.setGeometry(geometry);
    feature.setAttributes(QgsAttributes() << QVariant(1) << QVariant("river") << QVariant("river"));
    dataProvider->addFeatures(QgsFeatureList() << feature);
    vLayer->updateExtents();

    // 将图层添加到画布
    QgsProject::instance()->addMapLayer(vLayer);
    //mapCanvas->setExtent(vLayer->extent());
    layers.append(vLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// 创建临时多边形图层
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

    // 在图层中添加多边形
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

    // 将图层添加到画布
    QgsProject::instance()->addMapLayer(vLayer);
    //mapCanvas->setExtent(vLayer->extent());
    layers.append(vLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

// 进入添加点要素的编辑模式
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

// 进入添加线要素的编辑模式
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

// 进入多边形要素的编辑模式
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

// 结束编辑模式
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

// 编辑点样式
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

// 编辑线样式
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

// 编辑多边形样式
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

// 显示标签
void DemoLabel::showLabelTrigger()
{
    if (!judgeOneVectoerSelect(layerTreeView->selectedLayers()))
    {
        return;
    }
    labelLayer = layerTreeView->selectedLayers()[0];
    QgsVectorLayer* mLayer = (QgsVectorLayer*)labelLayer;

    // 首先是定义一个 QgsPalLayerSettings 变量，并启用他的属性设置
    QgsPalLayerSettings layerSettings;
    layerSettings.drawLabels = true;
    //根据API文档中的属性，进行自定义配置
    layerSettings.fieldName = mLayer->fields()[1].name(); // 设置Label图层与标注字段
    //layerSettings.fieldName = mLayer->fields().field("name").name();
    layerSettings.centroidWhole = true; // 设置位置参考的中心点（从整个特征计算or特征可见部分）
    layerSettings.isExpression = true;  // 标签由表达式字符串组成
    if (mLayer->geometryType() == QgsWkbTypes::PointGeometry)
    {
        layerSettings.placement = QgsPalLayerSettings::AroundPoint;   // 候选标签放置模式
    }
    else if (mLayer->geometryType() == QgsWkbTypes::LineGeometry)
    {
        layerSettings.placement = QgsPalLayerSettings::Line;   // 候选标签放置模式
    }
    else if (mLayer->geometryType() == QgsWkbTypes::PolygonGeometry)
    {
        layerSettings.placement = QgsPalLayerSettings::Horizontal;   // 候选标签放置模式
    }
    else
    {
        return;
    }
    layerSettings.yOffset = 2.5;    // 标签垂直偏移
    // Label 字体设置:轮廓buffer设置
    QgsTextBufferSettings buffersettings;
    buffersettings.setEnabled(true);
    buffersettings.setSize(1);
    buffersettings.setColor(QColor(255, 255, 0));
    //字体阴影
    QgsTextShadowSettings shadowsetings;
    shadowsetings.setEnabled(true);
    shadowsetings.setOffsetAngle(135);
    shadowsetings.setOffsetDistance(1);
    //字体格式
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

// 隐藏标签
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

// 进入信息识别模式
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

// 退出信息识别模式
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

// 显示要素信息
void DemoLabel::showInfoTrigger(QgsFeature f)
{
    //QgsVectorLayer* vlayer = (QgsVectorLayer*)infoLayer;
    //QgsFeature f =  vlayer->getFeature(ff);
    QgsFields field = f.fields(); // 保存要素属性
    QgsAttributes attribute = f.attributes(); // 保存要素属性变量
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

// 在状态栏中显示鼠标坐标
void DemoLabel::showMousePointTrigger(const QgsPointXY& p)
{
    lblCoordinate->setText(QString("x=%1, y=%2").arg(p.x()).arg(p.y()));
}

// 打开点要素属性输入窗口
void DemoLabel::addPointFeatureTrigger(const QgsPointXY& p)
{
    // 创建点
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

// 添加点要素到矢量图层
void DemoLabel::addPointFeatureConfirmTrigger(QMap<QString, QVariant> settings)
{
    //QMessageBox::information(this, "", settings["name"].toString());
    // 创建点
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
    // 开始编辑
    mVecLayer->startEditing();

    // 添加要素myFeature1
    dataProvider->addFeatures(QgsFeatureList() << myFeature);
    mVecLayer->commitChanges();
    mVecLayer->updateExtents();
    mapCanvas->refresh();
}

// 线要素取点
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

// 打开线要素属性输入窗口
void DemoLabel::addLineFeatureTrigger()
{
    addLineFlag = false;
    this->disconnect(editMapCamvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showLineTrigger(QgsPointXY)));
    if (addLine.size() < 2)
    {
        QMessageBox::information(this, "Failure", "No line.");
        return;
    }
    // 创建线
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

// 添加线要素到矢量图层
void DemoLabel::addLineFeatureConfirmTrigger(QMap<QString, QVariant> settings)
{
    // 创建线
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

    // 开始编辑
    mVecLayer->startEditing();

    // 添加要素myFeature1
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

// 显示拟绘制的线要素
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

// 多边形要素取点
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

// 打开多边形要素属性页面
void DemoLabel::addPolygonFeatureTrigger()
{
    addPolygonFlag = false;
    this->disconnect(editMapCamvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showPolygonTrigger(QgsPointXY)));
    if (addPolygon.size() < 3)
    {
        QMessageBox::information(this, "Failure", "No polygon.");
        return;
    }
    // 创建多边形
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

// 添加多边形要素到矢量图层
void DemoLabel::addPolygonFeatureConfirmTrigger(QMap<QString, QVariant> settings)
{
    // 创建多边形
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

    // 开始编辑
    mVecLayer->startEditing();

    // 添加要素myFeature1
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

// 显示拟绘制的多边形
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

// 对当前选中图层反加权插值
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

        // 添加栅格数据
        QgsProject::instance()->addMapLayer(rasLayer);   // 注册
        mapCanvas->setExtent(rasLayer->extent());        // 将画布范围设置为栅格图层范围
        layers.append(rasLayer);                         // 将栅格图层追加到链表中
        mapCanvas->setLayers(layers);                       // 将图层画到画布上
        mapCanvas->setVisible(true);
        mapCanvas->freeze(false);
        mapCanvas->refresh();                               // 更新画布
    }
    else
    {
        QMessageBox::information(this, "Failure", "Cannot interpolate.");
        return;
    }
}

// 对当前选中图层三角插值
void DemoLabel::tinInterpolateTrigger()
{
}

// ui布局
void DemoLabel::InitUI()
{
    InitMenuBar();
    InitToolBar();
    InitMapCanvas();
    InitInfoTable();
    InitStatusBar();
    // 栅格布局
    QWidget* centralWidget = this->centralWidget();
    QGridLayout* centralLayout = new QGridLayout(centralWidget);
    centralLayout->addWidget(layerTreeView, 0, 0, 2, 1);            // 图层管理器位置
    centralLayout->addWidget(mapCanvas, 0, 1, 2, 1);                // 画布位置 参数：初始行列，占位行列
    centralLayout->addWidget(editMapCamvas, 0, 1, 2, 1);
    centralLayout->addLayout(infoAreaLayout, 0, 2, 2, 2);
    editMapCamvas->stackUnder(mapCanvas);
	//窗口最大化
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowTitle(QString::fromLocal8Bit("DemoLabel"));

    // 初始化要素属性
    QTime randTime = QTime::currentTime();
    qsrand(randTime.msec() + randTime.second() * 1000);
    names = "qwertyuiopasdfghjklzxcvbnm";
    pointType = QList<QString>() << "city" << "peak" << "landmark";
    lineType = QList<QString>() << "river" << "road" << "pipeline";
    polygonType = QList<QString>() << "ocean" << "island" << "lake";
}

// 菜单栏初始化
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

// 工具栏初始化
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

// 初始化地图画布
void DemoLabel::InitMapCanvas()
{
    mapCanvas = new QgsMapCanvas();
    //this->setCentralWidget(mapCanvas);                  // 将地图画布放置在主窗口中心
    mapCanvas->setCanvasColor(QColor(255, 255, 255));   // 设置地图画布为白色
    mapCanvas->setVisible(true);
    mapCanvas->enableAntiAliasing(true);

    editMapCamvas = new QgsEditMapCanvas();
    editMapCamvas->setCanvasColor(QColor(0, 0, 0, 0));
    editMapCamvas->setVisible(true);
    editMapCamvas->enableAntiAliasing(true);

    //-----------------------------------

    // 初始化图层管理器
    layerTreeView = new QgsLayerTreeView(this);
    initLayerTreeView();
}

// 初始化图层管理器
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

    // 连接地图画布和图层管理器
    layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), mapCanvas, this);
    connect(QgsProject::instance(), SIGNAL(writeProject(QDomDocument&)), layerTreeCanvasBridge, SLOT(writeProject(QDomDocument&)));
    connect(QgsProject::instance(), SIGNAL(readProject(QDomDocument&)), layerTreeCanvasBridge, SLOT(readProject(QDomDocument&)));
}

// 设置属性表
void DemoLabel::InitInfoTable()
{
    infoAreaLayout = new QVBoxLayout();
    infoTable = new QTableWidget(0, 2);
    infoTable->setHorizontalHeaderLabels(QStringList() << "Attribute" << "Value");
    //infoTable->setFixedWidth(300);
    infoAreaLayout->addWidget(infoTable);
}

// 坐标显示
void DemoLabel::InitStatusBar()
{
    lblCoordinate = new QLabel;
    lblCoordinate->setMinimumSize(100, 20);
    lblCoordinate->setFrameShape(QFrame::WinPanel);
    lblCoordinate->setFrameShadow(QFrame::Sunken);
    ui.statusBar->addWidget(lblCoordinate);
    this->connect(mapCanvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showMousePointTrigger(QgsPointXY)));
}

// 判断选择的图层是否为矢量图层
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

// 返回正在添加的线要素
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

// 返回正在添加的多边形要素
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

// 实现反加权插值
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
    qint32 writeResult = gridWriter->writeFile(); // 成功返回0， 失败返回1
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

// 实现三角插值
QgsRasterLayer* DemoLabel::achieveTINInterpolate(QgsVectorLayer* layer)
{
    return nullptr;
}
