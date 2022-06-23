#include "QgsEditMapCanvas.h"
#include <qmessagebox.h>

QgsEditMapCanvas::QgsEditMapCanvas() : QgsMapCanvas()
{
}

QgsEditMapCanvas::~QgsEditMapCanvas()
{
}

void QgsEditMapCanvas::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QgsPointXY mCursorPoint = getCoordinateTransform()->toMapCoordinates(e->pos());
		emit xyClick(mCursorPoint);
		//QMessageBox::information(this, "", "leftpress");
	}
	if (e->button() == Qt::RightButton)
	{
		QgsPointXY mCursorPoint = getCoordinateTransform()->toMapCoordinates(e->pos());
		emit xyClickRight(mCursorPoint);
		//QMessageBox::information(this, "", "rightpress");
	}
	QgsMapCanvas::mousePressEvent(e);
}

void QgsEditMapCanvas::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QgsPointXY mCursorPoint = getCoordinateTransform()->toMapCoordinates(e->pos());
		emit xyRelease(mCursorPoint);
		//QMessageBox::information(this, "", "leftrelease");
	}
	if (e->button() == Qt::RightButton)
	{
		QgsPointXY mCursorPoint = getCoordinateTransform()->toMapCoordinates(e->pos());
		emit xyReleaseRight(mCursorPoint);
		//QMessageBox::information(this, "", "leftrelease");
	}
	QgsMapCanvas::mouseReleaseEvent(e);
}
