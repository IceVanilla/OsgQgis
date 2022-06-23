#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "GraphicsWindowQt.h"
#include "AddFeatureSettingBox.h"
#include "QgsEditMapCanvas.h"
#include "EditFeatureStyleBox.h"

#include <qdir.h>
#include <qmenu.h>              // QT菜单类
#include <qaction.h>            // QT行为类

#include <qgsrasterlayer.h>     // QGIS栅格图层
#include <qgsvectorlayer.h>     // QGIS矢量图层
#include <qgsmaplayer.h>        // QGIS图层
#include <qgsmapcanvas.h>       // QGIS画布

#include <qgslayertreeview.h>   // QGIS图层管理器
#include <qgslayertreemapcanvasbridge.h>    // 连接画布和图层管理器
#include <qgsmaptoolidentifyfeature.h>
#include <qgsmaptool.h>
#include <qgsmaptooldigitizefeature.h>
#include <qgsadvanceddigitizingdockwidget.h>
#include <qgspallabeling.h>
#include <qgsvectorlayerlabeling.h>
#include <qgstextrenderer.h>
#include <qgstextshadowsettings.h>
#include <qgshighlight.h>

#include <qgsinterpolator.h>
#include <qgsidwinterpolator.h>
#include <qgstininterpolator.h>
#include <qgsgridfilewriter.h>

#include <QLabel>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <QVBoxLayout>
#include <qpainter.h>
#include <qgraphicsscene.h>

class DemoLabel : public QMainWindow
{
    Q_OBJECT

public:
    DemoLabel(QWidget *parent = Q_NULLPTR);
	~DemoLabel();
	// 保持单一实例
	static DemoLabel* instance() { return sm_instance; }

private:
    Ui::QtWidgetsApplication1Class ui;

    // 单一实例
    static DemoLabel* sm_instance;

    // 菜单栏
    QMenu* fileMenu;
    QAction* openRasterFileAction;
    QAction* openVectorFileAction;
    QAction* saveVectorLayerAction;
    QMenu* editMenu;
    QAction* addPointLayerAction;
    QAction* addLineLayerAction;
    QAction* addPolygonLayerAction;
    QMenu* processMenu;
    QAction* idwInterpolateAction;
    QAction* tinInterpolateAction;

    QgsPointXY addPoint;
    QList<QgsPointXY> addLine;
    bool addLineFlag;
    QList<QgsPointXY> addPolygon;
    bool addPolygonFlag;

    // 工具栏
    QPushButton* btnAddPoint;
    QPushButton* btnAddLine;
    QPushButton* btnAddPolygon;
    QPushButton* btnSaveEdit;
    QPushButton* btnEditPointStyle;
    QPushButton* btnEditLineStyle;
    QPushButton* btnEditPolygonStyle;
    QPushButton* btnShowLabel;
    QPushButton* btnHiddenLabel;
    QPushButton* btnIdentifyOn;
    QPushButton* btnIdentifyOff;
    bool editFlag;
    bool infoFlag;
    QgsMapLayer* editLayer;
    QgsMapLayer* labelLayer;
    QgsMapLayer* infoLayer;
    QgsEditMapCanvas* editMapCamvas;
    QGraphicsScene* graphicsScene;

    // 地图画布
    QgsMapCanvas* mapCanvas;
    QList<QgsMapLayer*> layers;
    QgsMapToolIdentifyFeature* identifyTool;
    QgsHighlight* highLight;
    bool infoHighLightFlag;

    // 图层管理器
    QgsLayerTreeView* layerTreeView;
    QgsLayerTreeMapCanvasBridge* layerTreeCanvasBridge;

    // 属性表
    QVBoxLayout* infoAreaLayout;
    QTableWidget* infoTable;

    QLabel* lblCoordinate;
    QString names;
    QList<QString> pointType;
    QList<QString> lineType;
    QList<QString> polygonType;
    QgsHighlight* addedHighLight;
    QgsHighlight* addingHighLight;
    bool addedHighLightFlag;
    bool addingHighLightFlag;

public slots:
    void openRasterFileActionTrigger();
    void openVectorFileActionTrigger();
    void saveVectorLayerActionTrigger();
    void addPointLayerActionTrigger();
    void addLineLayerActionTrigger();
    void addPolygonLayerActionTrigger();

    void addPointTrigger();
    void addLineTrigger();
    void addPolygonTrigger();
    void saveEditTrigger();
    void editPointStyleTrigger();
    void editLineStyleTrigger();
    void editPolygonStyleTrigger();
    void showLabelTrigger();
    void hiddenLabelTrigger();
    void identifyStratTrigger();
    void identifyEndTrigger();
    void showInfoTrigger(QgsFeature f);

    void showMousePointTrigger(const QgsPointXY& p);
    void addPointFeatureTrigger(const QgsPointXY& p);
    void addPointFeatureConfirmTrigger(QMap<QString, QVariant> settings);
    void addLinePointTrigger(const QgsPointXY& p);
    void addLineFeatureTrigger();
    void addLineFeatureConfirmTrigger(QMap<QString, QVariant> settings);
    void showLineTrigger(const QgsPointXY& p);
    void addPolygonPointTrigger(const QgsPointXY& p);
    void addPolygonFeatureTrigger();
    void addPolygonFeatureConfirmTrigger(QMap<QString, QVariant> settings);
    void showPolygonTrigger(const QgsPointXY& p);

    void idwInterpolateTrigger();
    void tinInterpolateTrigger();

private:
	void InitUI();//界面初始化
    void InitMenuBar();
    void InitToolBar();
    void InitMapCanvas();
    void initLayerTreeView();       // 初始化图层管理器函数
    void InitInfoTable();
    void InitStatusBar();
    bool judgeOneVectoerSelect(QList<QgsMapLayer*> selectLayers);
    QgsFeature getAddLineFeature();
    QgsFeature getAddPolygonFeature();

    QgsRasterLayer* achieveIDWInterpolate(QgsVectorLayer* layer);
    QgsRasterLayer* achieveTINInterpolate(QgsVectorLayer* layer);
};
