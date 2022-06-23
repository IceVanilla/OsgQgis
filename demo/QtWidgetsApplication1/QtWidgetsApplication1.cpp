#include "QtWidgetsApplication1.h"
#include <osg/TexGen>
#include <qmessagebox.h>

#include <qgridlayout.h>
#include <qmessagebox.h>

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	InitOSG();
	InitOsgearth();
	InitQgs();
	InitUI();
	InitTimer();
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
}

void QtWidgetsApplication1::InitOSG()
{
	viewer = new osgViewer::Viewer;
	// ����ģ��
	root = new osg::Group;
	// ��ʾ .earth �ļ��еĵ���ģ��
	earthNode = new osg::Node;
	QString earthFilePath = "C:/Users/COLORFUL/Documents/Data/osgearth/tests/simple.earth";
	earthNode = osgDB::readNodeFile(earthFilePath.toStdString());
	//osg::ref_ptr<osg::Node> n = osgDB::readNodeFile(earthFilePath.toStdString());
	if (!earthNode.valid())
	{
		QMessageBox::information(this, "", "");
	}

	//��ȡ��Ļ�ֱ��� ����
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if (!wsi)
		return;
	unsigned int width, height;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
	//����ͼ�λ�������
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowDecoration = false;//�����Ƿ���ʾ���ڵ�����
	traits->x = 0;
	traits->y = 0;
	traits->width = width;
	traits->height = height;
	traits->doubleBuffer = true;//����ͼ�δ����Ƿ�ʹ��˫����

	//���������
	camera = new osg::Camera;
	camera->setGraphicsContext(new osgQt::GraphicsWindowQt(traits.get()));
	camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, width, height));
	camera->setProjectionMatrixAsPerspective(30.0f, (double(traits->width)) / (double(traits->height)), 1.0f, 10000.0f);

	//������Ⱦ��
	viewer->setCamera(camera);
	viewer->setSceneData(root);
	viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);//����Ϊ���߳�
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
}

void QtWidgetsApplication1::InitUI()
{
	// ui����
	osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>(camera->getGraphicsContext());
	QWidget* osg_widget = (QWidget*)(gw->getGLWidget());
	//this->setCentralWidget(osg_widget);
	QWidget* centralWidget = this->centralWidget();
	QGridLayout* centralLayout = new QGridLayout(centralWidget);
	centralLayout->addWidget(mapCanvas, 0, 0);
	centralLayout->addWidget(osg_widget, 0, 1);
	centralLayout->setColumnStretch(0, 1);
	centralLayout->setColumnStretch(1, 1);
	centralLayout->setHorizontalSpacing(5);
	//�������
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowTitle(QString::fromLocal8Bit("���ֵ���"));
}

void QtWidgetsApplication1::InitTimer()
{
	_timer = new QTimer;
	QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
	_timer->start(10);
}

void QtWidgetsApplication1::InitOsgearth()
{
	//mapnode��ʼ��
	mapNode = osgEarth::MapNode::findMapNode(earthNode.get());
	earthForm = new osg::MatrixTransform;
	//osgearth������
	em = new osgEarth::Util::EarthManipulator;
	if (mapNode.valid())
	{
		em->setNode(mapNode);
	}

	em->getSettings()->setArcViewpointTransitions(true);
	viewer->setCameraManipulator(em);

	//��ȡ����뾶 �����ӵ�
	//mapNode->getMap()->getSRS()->getEllipsoid().getRadiusEquator();
	double earth_R = mapNode->getMap()->getSRS()->getEllipsoid().getRadiusEquator();
	const char* viewPointName = QString::fromLocal8Bit("����").toStdString().c_str();
	em->setViewpoint(osgEarth::Viewpoint(viewPointName, 112.44, 33.75, 0.0, 0.0, -90.0, 5 * earth_R), 5);
	//��ʼ�����
	InitSky();
}

void QtWidgetsApplication1::InitSky()
{
	//��ȡ��ǰʱ�� ��ʼ�����
	now_time = time(0);
	t_tm = localtime(&now_time);
	osgEarth::DateTime cur_date_time(now_time);
	osgEarth::Ephemeris* ephemeris = new osgEarth::Ephemeris;

	//���ú�ҹ�����̶�
	osgEarth::Util::SkyOptions skyOptions;
	skyOptions.ambient() = 0.3;

	m_pSkyNode = osgEarth::SkyNode::create(skyOptions);
	m_pSkyNode->setName("SkyNode");
	m_pSkyNode->setEphemeris(ephemeris);
	m_pSkyNode->setDateTime(cur_date_time);
	viewer->setLightingMode(osg::View::SKY_LIGHT);
	m_pSkyNode->attach(viewer, 0);
	m_pSkyNode->setLighting(true);

	m_pSkyNode->addChild(mapNode);
	root->addChild(m_pSkyNode);
}

void QtWidgetsApplication1::InitQgs()
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

void QtWidgetsApplication1::updateFrame()
{
	viewer->frame();
}