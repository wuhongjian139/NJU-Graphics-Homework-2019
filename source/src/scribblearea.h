#pragma once

#include "proc.h"

#include <QRect>
#include <QImage>
#include <QMainWindow>
#include <QToolButton>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QStatusBar>
#include <QColorDialog> 
#include <QFileDialog>
#include <QGridLayout>
#pragma execution_character_set("UTF-8")
class ScribbleArea : public QMainWindow {
	Q_OBJECT

public:
	ScribbleArea(QWidget* parent = nullptr);
	enum MouseState { line = 0, curve, ellipse, polygon, trans, rotate, scale, clip } mouseState;
private:
	void clsFlag();
	cgcore::Shape* curShape;
	QList<cgcore::Shape> funcShapes;
	bool isDragging, putAnchor;
	QList<cgcore::Shape*> shapes;
	bool  Isdda, Isbezier, Iscohen_sutherland;
	bool justDbClicked, prepareAnchor, renderOver, isFirstDbPoint;
	QRgb color;
	QPoint curPos, anchorPos, start, end;
	QList<QToolButton*>btns;
	QToolButton* resetbtn, * colorbtn, * savebtn, * algoBtn,
		* linebtn, * curvebtn, * ellipsebtn, * polygonbtn,
		* transbtn, * rotatebtn, * scalebtn, * clipbtn;
	QStatusBar* statusbar;
	const int btnCounter = 12;
	void updateLayout();
	int scaleK;
	QRect upBorder, downBorder, leftBorder, rightBorder;
	void autoResize();
	QImage imgBuffer, instBuffer;
	QColorDialog colordialog;
	QFileDialog filedialog;
	QWidget algoWidget;
	QString info;
protected:
	void resizeEvent(QResizeEvent* event) override;
	void moveEvent(QMoveEvent* event)override;
	void closeEvent(QCloseEvent* event)override;
	void paintEvent(QPaintEvent* event)override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event)override;
	void wheelEvent(QWheelEvent* event)override;
};

int throwErrorBox(const char* windowTitle, const char* message);

QSize getDesktopSize();
