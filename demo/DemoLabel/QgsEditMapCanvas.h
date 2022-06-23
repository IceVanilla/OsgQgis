#pragma once

#ifndef QGSEDITMAPCANVAS
#define QGSEDITMAPCANVAS

#include <qgsmapcanvas.h>

class QgsEditMapCanvas : public QgsMapCanvas
{
	Q_OBJECT
public:
	/*using QgsMapCanvas::QgsMapCanvas;*/
	QgsEditMapCanvas();
	~QgsEditMapCanvas();

signals:
	void xyClick(const QgsPointXY& p);
	void xyRelease(const QgsPointXY& p);
	void xyClickRight(const QgsPointXY& p);
	void xyReleaseRight(const QgsPointXY& p);

protected:
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
};

#endif // !QGSEDITMAPCANVAS
