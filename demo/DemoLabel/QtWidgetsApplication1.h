#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "GraphicsWindowQt.h"
#include "AddFeatureSettingBox.h"
#include "QgsEditMapCanvas.h"
#include "EditFeatureStyleBox.h"

#include <qdir.h>
#include <qmenu.h>              // QT�˵���
#include <qaction.h>            // QT��Ϊ��

#include <qgsrasterlayer.h>     // QGISդ��ͼ��
#include <qgsvectorlayer.h>     // QGISʸ��ͼ��
#include <qgsmaplayer.h>        // QGISͼ��
#include <qgsmapcanvas.h>       // QGIS����

#include <qgslayertreeview.h>   // QGISͼ�������
#include <qgslayertreemapcanvasbridge.h>    // ���ӻ�����ͼ�������
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
	// ���ֵ�һʵ��
	static DemoLabel* instance() { return sm_instance; }

private:
    Ui::QtWidgetsApplication1Class ui;

    // ��һʵ��
    static DemoLabel* sm_instance;

    // �˵���
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

    // ������
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

    // ��ͼ����
    QgsMapCanvas* mapCanvas;
    QList<QgsMapLayer*> layers;
    QgsMapToolIdentifyFeature* identifyTool;
    QgsHighlight* highLight;
    bool infoHighLightFlag;

    // ͼ�������
    QgsLayerTreeView* layerTreeView;
    QgsLayerTreeMapCanvasBridge* layerTreeCanvasBridge;

    // ���Ա�
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
	void InitUI();//�����ʼ��
    void InitMenuBar();
    void InitToolBar();
    void InitMapCanvas();
    void initLayerTreeView();       // ��ʼ��ͼ�����������
    void InitInfoTable();
    void InitStatusBar();
    bool judgeOneVectoerSelect(QList<QgsMapLayer*> selectLayers);
    QgsFeature getAddLineFeature();
    QgsFeature getAddPolygonFeature();

    QgsRasterLayer* achieveIDWInterpolate(QgsVectorLayer* layer);
    QgsRasterLayer* achieveTINInterpolate(QgsVectorLayer* layer);
};
