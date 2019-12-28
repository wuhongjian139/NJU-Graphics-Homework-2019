#include "scribblearea.h"
#include <QErrorMessage>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QDebug>
using namespace std;
using namespace cgcore;

#define ANCHORPIX QPixmap(":/Resources/anchor.png").scaled(20, 20)

int throwErrorBox(const char* windowTitle, const char* message) {
	QErrorMessage dialog;
	dialog.setModal(true);
	dialog.setWindowTitle(QObject::tr(windowTitle));
	dialog.showMessage(QObject::tr(message));
	return dialog.exec();
}

QSize getDesktopSize() {
	return QApplication::desktop()->size();
}

ScribbleArea::ScribbleArea(QWidget* parent) :QMainWindow(parent), color(Qt::black), isDragging(false), putAnchor(false), prepareAnchor(false),
Isdda(false), Isbezier(true), isFirstDbPoint(true),
Iscohen_sutherland(false), justDbClicked(false), curShape(nullptr) {
	for (int i = 0; i < btnCounter; i++) {
		btns.push_back(new QToolButton(this));
		btns.at(i)->setStyleSheet(QString("QToolButton{border:2px groove gray;border-radius:10px;padding:2px 4px;background-color: rgb(242, 252, 252)}""QToolButton:hover{border:4px groove gray;border-radius:10px;padding:2px 4px;background-color: rgb(242, 252, 252)}""QToolButton:pressed{border:5px groove gray;border-radius:10px;padding:2px 4px;background-color: rgb(242, 252, 252)}"));
	}
	resetbtn = btns.at(11); resetbtn->setText("重置");
	connect(resetbtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			for (auto& i : shapes) {
				delete i;
			}
			shapes.clear();
			imgBuffer = instBuffer = QImage(size(), QImage::Format::Format_RGB888);
			update();
		}
	);
	colorbtn = btns.at(10);  colorbtn->setText("换色");
	connect(colorbtn, &QToolButton::clicked, this, [=]() {colordialog.show(); });
	connect(
		&colordialog, &QColorDialog::colorSelected,
		this, [=](const QColor& _color) {
			colorbtn->setStyleSheet(
				QString("QToolButton{border:2px groove gray;border-radius:10px;padding:2px 4px; background-color:rgba(%1,%2,%3,50)}").arg(_color.red()).arg(_color.green()).arg(_color.blue()) +
				QString("QToolButton:hover{border:4px groove gray;border-radius:10px;padding:2px 4px;background-color:rgba(%1,%2,%3,50)}").arg(_color.red()).arg(_color.green()).arg(_color.blue()) +
				QString("QToolButton:pressed{border:5px groove gray;border-radius:10px;padding:2px 4px;background-color:rgba(%1,%2,%3,50)}").arg(_color.red()).arg(_color.green()).arg(_color.blue())
			);
			color = _color.rgb();
		}
	);
	colordialog.colorSelected(Qt::black);
	savebtn = btns.at(9); savebtn->setText("保存");
	filedialog.setModal(true);//  要求模态对话框
	connect(
		savebtn, &QToolButton::clicked,
		&filedialog, [=] {
			QString saveName = filedialog.getSaveFileName(
				nullptr, "", QApplication::applicationDirPath(), QString("*.bmp")
			);
			if (QFileInfo(saveName).fileName().isEmpty()) {
				saveName = "tmp" + saveName;
			}
			if (QFileInfo(saveName).suffix().isEmpty()) {
				saveName += ".bmp";
			}
			imgBuffer.save(saveName);
		}
	);
	algoBtn = btns.at(8); algoBtn->setText("算法");
	QToolButton* bresenham = new QToolButton(&algoWidget),
		* dda = new QToolButton(&algoWidget),
		* bezier = new QToolButton(&algoWidget),
		* bspline = new QToolButton(&algoWidget),
		* cohen_sutherland = new QToolButton(&algoWidget),
		* liang_barsky = new QToolButton(&algoWidget);
	bresenham->setText("bresenham"); bresenham->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	dda->setText("dda"); dda->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	bezier->setText("bezier"); bezier->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	bspline->setText("bspline"); bspline->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	cohen_sutherland->setText("cohen_sutherland"); cohen_sutherland->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	liang_barsky->setText("liang_barsky"); liang_barsky->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(bresenham, &QToolButton::clicked, this, [=]() {Isdda = false; throwErrorBox("信息", "选择成功！"); });
	connect(dda, &QToolButton::clicked, this, [=]() {Isdda = true; throwErrorBox("信息", "选择成功！"); });
	connect(bezier, &QToolButton::clicked, this, [=]() {Isbezier = true; throwErrorBox("信息", "选择成功！"); });
	connect(bspline, &QToolButton::clicked, this, [=]() {Isbezier = false; throwErrorBox("信息", "选择成功！"); });
	connect(liang_barsky, &QToolButton::clicked, this, [=]() {Iscohen_sutherland = false; throwErrorBox("信息", "选择成功！"); });
	connect(cohen_sutherland, &QToolButton::clicked, this, [=]() {Iscohen_sutherland = true; throwErrorBox("信息", "选择成功！"); });
	QGridLayout* l = new QGridLayout(&algoWidget);
	for (int i = 0; i < 2; i++)
		l->setRowStretch(i, 1);
	for (int i = 0; i < 3; i++)
		l->setColumnStretch(i, 1);
	l->addWidget(bresenham, 0, 0); l->addWidget(dda, 0, 1); l->addWidget(bezier, 0, 2);
	l->addWidget(bspline, 1, 0); l->addWidget(cohen_sutherland, 1, 1); l->addWidget(liang_barsky, 1, 2);
	algoWidget.setLayout(l);
	algoWidget.setWindowModality(Qt::ApplicationModal);
	connect(
		algoBtn, &QToolButton::clicked, this, [=]() {
			algoWidget.show();
		}
	);
	linebtn = btns.at(0);  linebtn->setText("线段");
	connect(linebtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = line;
			info = "直线绘制，按下鼠标左键、拖曳以继续";
		}
	);
	curvebtn = btns.at(3);  curvebtn->setText("曲线");
	connect(curvebtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = curve;
			info = "曲线绘制，左键*双击*以继续，右键*双击*以结束";
		}
	);
	ellipsebtn = btns.at(1); ellipsebtn->setText("椭圆");
	connect(ellipsebtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = ellipse;
			info = "椭圆绘制，按下鼠标左键、拖曳以继续";
		}
	);
	polygonbtn = btns.at(2); polygonbtn->setText("多边形");
	connect(polygonbtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = polygon;
			info = "多边形绘制，左键*双击*以继续，右键*双击*以结束";
		}
	);
	transbtn = btns.at(4);  transbtn->setText("平移");
	connect(transbtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			setCursor(Qt::OpenHandCursor);
			mouseState = trans;
			info = "图元平移，在图元上按下鼠标左键，拖曳鼠标以继续";
		}
	);
	rotatebtn = btns.at(5);  rotatebtn->setText("旋转锚");
	connect(rotatebtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = rotate;
			prepareAnchor = true;
			setCursor(QCursor(ANCHORPIX, 0, 20));
			info = "图元旋转，单击放下旋转中心锚点，点击一下目标图元，转动鼠标滚轮以继续";
		}
	);
	scalebtn = btns.at(6);  scalebtn->setText("缩放锚");
	connect(scalebtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = scale;
			prepareAnchor = true;
			setCursor(QCursor(ANCHORPIX, 0, 20));
			info = "图元缩放，单击放下缩放中心锚点，点击一下目标图元，转动鼠标滚轮以继续";
		}
	);
	clipbtn = btns.at(7); clipbtn->setText("裁剪");
	connect(clipbtn, &QToolButton::clicked, this,
		[=]() {
			clsFlag();
			mouseState = clip;
			info = "直线裁剪";
		}
	);

	statusBar()->showMessage("就绪");
	linebtn->click();
	setMouseTracking(true);
}

void ScribbleArea::updateLayout() {
	int b = 10;
	int h = height() / btnCounter;
	int	w = width() / btnCounter - b - 1;
	for (size_t i = 0; i < btnCounter; i++) {
		btns.at(i)->resize(w, h);
		btns.at(i)->move(b * (i + 1) + w * i, b);
		btns.at(i)->setFont(QFont("Arial", std::min(sqrt(h), sqrt(w)) + 3, QFont::Bold));
	}
}

void ScribbleArea::autoResize() {
	if (upBorder.contains(curPos)) {
		if (y() - scaleK > 0 && scaleK + height() < getDesktopSize().height() - 100) {
			move(x(), y() - scaleK);
			resize(width(), scaleK + height());
		}
	}
	else if (downBorder.contains(curPos)) {
		if (scaleK + height() < getDesktopSize().height() - 100) {
			resize(width(), scaleK + height());
		}
	}
	if (leftBorder.contains(curPos)) {
		if (x() - scaleK > 0 && width() + scaleK < getDesktopSize().width()) {
			move(x() - scaleK, y());
			resize(width() + scaleK, height());
		}
	}
	else if (rightBorder.contains(curPos)) {
		if (x() > 0 && x() + width() + scaleK < getDesktopSize().width()) {
			resize(width() + scaleK, height());
		}
	}
}

void ScribbleArea::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);
	scaleK = width() / 64;
	updateLayout();
	upBorder.setCoords(0, 0, width(), scaleK);
	downBorder.setCoords(0, height() - scaleK, width(), height());
	leftBorder.setCoords(0, 0, scaleK, height());
	rightBorder.setCoords(width() - scaleK, 0, width() - scaleK, height());
	imgBuffer = instBuffer = QImage(size(), QImage::Format::Format_RGB888);
	imgBuffer.fill(Qt::white);
	instBuffer.fill(Qt::white);
	update();
}

void ScribbleArea::moveEvent(QMoveEvent* event) {
	int lastX = x(), lastY = y();
	QWidget::moveEvent(event);
	updateLayout();
}


void ScribbleArea::wheelEvent(QWheelEvent* event) {
	switch (mouseState) {
	case rotate: {
		if (curShape == nullptr)break;
		curShape->angle += event->delta() / 30;
		curShape->ctrlp.assign(curShape->_ctrlp.begin(), curShape->_ctrlp.end());
		curShape->rotate(anchorPos.x(), anchorPos.y(), curShape->angle);// 转到新位置
		break;
	}case scale: {
		if (curShape == nullptr)break;
		curShape->angle += 0.05 * (event->delta() / 120);
		if (curShape->angle <= 0)
			curShape->angle = 0.01;
		//qDebug() << curShape->angle << endl;
		curShape->ctrlp.assign(curShape->_ctrlp.begin(), curShape->_ctrlp.end());
		curShape->scale(anchorPos.x(), anchorPos.y(), curShape->angle, curShape->angle);// 转到新位置
		break;
	}
	default: {
		break;
	}
	}
	update();
	return QMainWindow::wheelEvent(event);
}

void ScribbleArea::closeEvent(QCloseEvent* event) {
	event->accept();
}

void ScribbleArea::clsFlag() {
	if ((mouseState == rotate || mouseState == scale) && curShape != nullptr && curShape->_ctrlp.size() > 0) {//  保存记录，只存一次，因为有误差累积
		curShape->_ctrlp.resize(curShape->ctrlp.size());
		curShape->_ctrlp.assign(curShape->ctrlp.begin(), curShape->ctrlp.end());
		//curShape->_ctrlp.clear();
		if (mouseState == scale)
			curShape->angle = 1;
		else
			curShape->angle = 0;
	}
	isFirstDbPoint = true;
	isDragging = false;
	putAnchor = false;
	prepareAnchor = false;
	Isdda = false;
	Isbezier = true;
	Iscohen_sutherland = false;
	justDbClicked = false;
	curShape = nullptr;
	setCursor(Qt::ArrowCursor);
	update();
}

void ScribbleArea::mousePressEvent(QMouseEvent* event) {
	if (event->button() != Qt::LeftButton)
		return;
	start = end = curPos = event->pos();
	switch (mouseState) {
	case line: {
		isDragging = true;
		curShape = new Shape();
		curShape->sType = curShape->line;
		if (Isdda)
			curShape->aType = curShape->dda;
		else
			curShape->aType = curShape->bresenham;
		curShape->color = color;
		curShape->ctrlp.resize(2, curPos);
		break;
	}case ellipse: {
		start = end = event->pos();
		isDragging = true;
		curShape = new Shape();
		curShape->sType = curShape->ellipse;
		curShape->color = color;
		curShape->ctrlp.resize(2);
		curShape->ctrlp[0] = QPoint((start.x() + end.x()) / 2, (start.y() + end.y()) / 2);
		curShape->ctrlp[1] = QPoint(abs(start.x() - end.x()) / 2, abs(start.y() - end.y()) / 2);
		break;
	}case polygon: case curve: {
		isDragging = true;
		break;
	}case trans: {
		isDragging = true;
		for (auto& i : shapes) {
			if (i->contains(curPos)) {
				setCursor(Qt::ClosedHandCursor);
				curShape = i; break;
			}
		}
		break;
	}case rotate: {
		if (prepareAnchor) {
			prepareAnchor = false;
			anchorPos = curPos;
			putAnchor = true;
			setCursor(Qt::CursorShape::ArrowCursor);
		}
		else {
			isDragging = true;
			for (auto& i : shapes) {
				if (i->contains(curPos)) {
					setCursor(Qt::CrossCursor);
					curShape = i;
					curShape->angle = 0;
					curShape->_ctrlp.resize(curShape->ctrlp.size());
					curShape->_ctrlp.assign(curShape->ctrlp.begin(), curShape->ctrlp.end());
					break;
				}
			}
		}
		break;
	}case scale: {
		if (prepareAnchor) {
			prepareAnchor = false;
			anchorPos = curPos;
			putAnchor = true;
			setCursor(Qt::CursorShape::ArrowCursor);
		}
		else {
			isDragging = true;
			for (auto& i : shapes) {
				if (i->contains(curPos)) {
					setCursor(Qt::CrossCursor);
					curShape = i;
					curShape->angle = 1;
					curShape->_ctrlp.resize(curShape->ctrlp.size());
					curShape->_ctrlp.assign(curShape->ctrlp.begin(), curShape->ctrlp.end());
					break;
				}
			}
		}
		break;
	}case clip: {
		isDragging = true;
		funcShapes.push_back(Shape());
		funcShapes[0].color = Qt::red;
		funcShapes[0].sType = funcShapes[0].polygon;
		funcShapes[0].ctrlp.resize(4);
		funcShapes[0].ctrlp[0] = start;
		funcShapes[0].ctrlp[3] = QPoint(start.x(), end.y());
		funcShapes[0].ctrlp[1] = QPoint(end.x(), start.y());
		funcShapes[0].ctrlp[2] = end;
		break;
	}
	default: {
		break;
	}
	}
	update();
}

void ScribbleArea::mouseMoveEvent(QMouseEvent* event) {
	statusBar()->showMessage(QString("【当前状态：%3】【当前鼠标位置：(%1,%2)】").arg(event->pos().x()).arg(event->pos().y()).arg(info));
	curPos = event->pos();
	if (!isDragging) {
		return QMainWindow::mouseMoveEvent(event);
	}
	if (event->buttons() != Qt::LeftButton) {
		if (mouseState == polygon && justDbClicked) {
			curShape->ctrlp.back() = curPos;
			update();
		}
		else if (mouseState == curve && justDbClicked) {
			curShape->ctrlp.back() = curPos;
			update();
		}
		return QMainWindow::mouseMoveEvent(event);
	}

	switch (mouseState) {
	case line: {
		curShape->ctrlp[1] = curPos;
		break;
	}case ellipse: {
		end = event->pos();
		curShape->ctrlp[0] = QPoint((start.x() + end.x()) / 2, (start.y() + end.y()) / 2);
		curShape->ctrlp[1] = QPoint(abs(start.x() - end.x()) / 2, abs(start.y() - end.y()) / 2);
		break;
	}case polygon: case curve: {
		break;
	}case trans: {
		if (curShape == nullptr)break;
		QPoint r = start - end;
		curShape->translate(r.x(), r.y());//先移回去
		end = curPos;
		r = end - start;
		curShape->translate(r.x(), r.y());//再移过去
		break;//为什么呢，因为鼠标单步移动的整型向量会出现误差累积
	}
				 //改用滚轮交互。搞不定向量夹角的精度问题
				 //case rotate: {\
			 	//	if (curShape == nullptr)break;
				 //	//  计算点和向量构成的三角形的点所在的夹角，but，where is eigen? 下面手算：
				 //	float _x1, _y1, _x2, _y2;
				 //	_x1 = start.x() - anchorPos.x(); _y1 = start.y() - anchorPos.y();
				 //	_x2 = end.x() - anchorPos.x(); _y2 = end.y() - anchorPos.y();
				 //	float _angle = (_x1 * _x2 + _y1 * _y2) / (sqrt(_x1 * _x1 + _y1 * _y1) * sqrt(_x2 * _x2 + _y2 * _y2));
				 //	curShape->rotate(anchorPos.x(), anchorPos.y(), (int)(180.0 / 3.14 * acosf(_angle)) % 180);// 转回起始位置
				 //	end = curPos;
				 //	_x2 = start.x() - anchorPos.x(); _y2 = start.y() - anchorPos.y();
				 //	_x1 = end.x() - anchorPos.x(); _y1 = end.y() - anchorPos.y();
				 //	_angle = (_x1 * _x2 + _y1 * _y2) / (sqrt(_x1 * _x1 + _y1 * _y1) * sqrt(_x2 * _x2 + _y2 * _y2));
				 //	qDebug() << "angle=" << _angle;
				 //	curShape->rotate(anchorPos.x(), anchorPos.y(), (int)(180.0/3.14*acosf(_angle))%180);// 转到新位置
				 //	break;
				 //}case scale: {
				 //	if (curShape == nullptr)break;
				 //	QPoint r = start - end;
				 //	curShape->translate(r.x(), r.y());
				 //	end = curPos;
				 //	r = end - start;
				 //	curShape->translate(r.x(), r.y());
				 //	break;
				 //}
	case clip: {
		end = curPos;
		funcShapes[0].ctrlp[0] = start;
		funcShapes[0].ctrlp[3] = QPoint(start.x(), end.y());
		funcShapes[0].ctrlp[1] = QPoint(end.x(), start.y());
		funcShapes[0].ctrlp[2] = end;
		break;
	}
	default: {
		break;
	}
	}
	update();
	QMainWindow::mouseMoveEvent(event);
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() != Qt::LeftButton)
		return QMainWindow::mouseReleaseEvent(event);
	switch (mouseState) {
	case line: {
		isDragging = false;
		shapes.push_back(curShape);
		curShape = nullptr;
		break;
	}case ellipse: {
		end = event->pos();
		isDragging = false;
		curShape->ctrlp[0] = QPoint((start.x() + end.x()) / 2, (start.y() + end.y()) / 2);
		curShape->ctrlp[1] = QPoint(abs(start.x() - end.x()) / 2, abs(start.y() - end.y()) / 2);
		shapes.push_back(curShape);
		curShape = nullptr;
		break;
	}case polygon:case curve: {
		break;
	}case trans: {
		curShape = nullptr;
		isDragging = false;
		setCursor(Qt::OpenHandCursor);
		break;
	}case rotate: {
		break;
	}case scale: {
		break;
	}case clip: {
		isDragging = false;
		end = curPos;
		int xmin, ymin, xmax, ymax;
		if (start.x() < end.x()) {
			xmin = start.x(); xmax = end.x();
		}
		else {
			xmax = start.x(); xmin = end.x();
		}
		if (start.y() < end.y()) {
			ymin = start.y(); ymax = end.y();
		}
		else {
			ymax = start.y(); ymin = end.y();
		}
		QRect tmp(xmin, ymin, xmax - xmin, ymax - ymin);
		for (auto& i : shapes) {
			if (i->sType == i->line && i->boundingRect.intersects(tmp)) {
				curShape = i; break;
			}
		}
		if (curShape == nullptr) {
			funcShapes.clear();
			break;
		}
		if (Iscohen_sutherland) {
			curShape->clip(xmin, ymin, xmax, ymax, curShape->cohen_sutherland);
		}
		else {
			curShape->clip(xmin, ymin, xmax, ymax, curShape->liang_barsky);
		}
		funcShapes.clear();
		curShape = nullptr;
		break;
	}
	default: {
		break;
	}
	}
	update();
	QMainWindow::mouseReleaseEvent(event);
}

void ScribbleArea::mouseDoubleClickEvent(QMouseEvent* event) {
	if (mouseState == polygon) {
		curPos = event->pos();
		justDbClicked = true;
		if (event->button() == Qt::LeftButton) {
			isDragging = true;
			if (curShape == nullptr) {
				curShape = new Shape();
				curShape->sType = curShape->polygon;
				if (Isdda)
					curShape->aType = curShape->dda;
				else
					curShape->aType = curShape->bresenham;
				curShape->color = color;
			}
			if (isFirstDbPoint) { //  需要多一个控制点来做实时索引
				curShape->ctrlp.push_back(curPos); isFirstDbPoint = false;
			}
			curShape->ctrlp.push_back(curPos);
		}
		else if (event->button() == Qt::RightButton) {
			isDragging = false;
			justDbClicked = false;
			curShape->ctrlp.back() = curPos;
			shapes.push_back(curShape);
			curShape = nullptr;
			isFirstDbPoint = true;
		}
		else
			return QMainWindow::mouseDoubleClickEvent(event);
		update();
	}
	else if (mouseState == curve) {
		curPos = event->pos();
		justDbClicked = true;
		if (event->button() == Qt::LeftButton) {
			isDragging = true;
			if (curShape == nullptr) {
				curShape = new Shape();
				curShape->sType = curShape->curve;
				//qDebug() << Isbezier;
				if (Isbezier)
					curShape->aType = curShape->bezier;
				else
					curShape->aType = curShape->bspline;
				curShape->color = color;
			}
			//  这里原来直接push了两次，对多边形而言看不出影响
			//  但是对于曲线来说，控制点数量直接影响到最终结果
			//  真是一个隐蔽的错误
			if (isFirstDbPoint) { //  需要多一个控制点来做实时索引
				curShape->ctrlp.push_back(curPos); isFirstDbPoint = false;
			}
			curShape->ctrlp.push_back(curPos);
		}
		else if (event->button() == Qt::RightButton) {
			isDragging = false;
			justDbClicked = false;
			curShape->ctrlp.back() = curPos;
			shapes.push_back(curShape);
			curShape = nullptr;
			isFirstDbPoint = true;
			//qDebug() << "完成右键双击"<<endl;
		}
		else
			return QMainWindow::mouseDoubleClickEvent(event);;
		update();
	}
	QMainWindow::mouseDoubleClickEvent(event);
}

void ScribbleArea::paintEvent(QPaintEvent* event) {
	imgBuffer.fill(Qt::white);
	QPainter painter(this);
	for (auto& i : shapes) {
		if (i != curShape)
			i->render(imgBuffer);
	}
	if (!isDragging) {
		curShape = nullptr;
		painter.drawImage(QPoint(0, 0), imgBuffer);
	}
	else {
		if (curShape != nullptr) {
			curShape->render(instBuffer);
		}
		for (auto& i : funcShapes) {
			i.render(instBuffer);
		}
		painter.drawImage(QPoint(0, 0), instBuffer);
	}
	if (putAnchor) {
		painter.drawPixmap(anchorPos - QPoint(0, 20), ANCHORPIX);
	}
	instBuffer = imgBuffer;
}
