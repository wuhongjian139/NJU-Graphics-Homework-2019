#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <QPoint>
#include <QRgb>
//  用QRect辅助鼠标定位图元
//  定位图元的标准做法是用BSP或者四叉树之类的
//  不过Qt没暴露多少BSP的接口，我也懒得自己写了
//  所以采用效率最low的遍历所有图元的方法定位鼠标
#include <QRect>
#include <QImage>
#pragma execution_character_set("UTF-8")
#define ERRORINT -0x3f3f3f3f
namespace cgcore {
	class Proc {
	public:
		/*向buffer填充构成直线(x1,y1)-(x2,y2)的点，clear变量控制是否清空帧缓存*/
		static void drawLineByDDA(
			int x1, int y1, int x2, int y2, std::vector<QPoint>& buffer, bool clear = true
		);
		/*向buffer填充构成直线(x1,y1)-(x2,y2)的点，clear变量控制是否清空帧缓存*/
		static void drawLineByBresenham(
			int x1, int y1, int x2, int y2, std::vector<QPoint>& buffer, bool clear = true
		);
		/*向buffer填充构成多边形{xi,yi}的点*/
		static void drawPolygonByDDA(
			const std::vector<int>& xs, const std::vector<int>& ys, std::vector<QPoint>& buffer
		);
		/*向buffer填充构成多边形{xi,yi}的点*/
		static void drawPolygonByBresenham(
			const std::vector<int>& xs, const std::vector<int>& ys, std::vector<QPoint>& buffer
		);
		/*向buffer填充构成椭圆{xi,yi}的点*/
		static void drawEllipse(
			int x0, int y0, int rx, int ry, std::vector<QPoint>& buffer
		);
		/*向buffer填充构成贝塞尔曲线{xi,yi}的点*/
		static void drawCurveByBezier(
			const std::vector<int>& xs, const std::vector<int>& ys, std::vector<QPoint>& buffer
		);
		/*向buffer填充构成B样条曲线{xi,yi}的点*/
		static void drawCurveByBSpline(
			const std::vector<int>& xs, const std::vector<int>& ys, std::vector<QPoint>& buffer
		);

		/*修改ctrlp为包含在矩形(x1,y1)(x2,y2)中的线段端点*/
		static void clipByCohenSutherland(
			int x1, int y1, int x2, int y2, std::vector<QPoint>& ctrlp
		);
		/*修改ctrlp为包含在矩形(x1,y1)(x2,y2)中的线段端点*/
		static void clipByLiangBarsky(
			int x1, int y1, int x2, int y2, std::vector<QPoint>& ctrlp
		);
		/*将ctrlbuffer中的点平移(x,y)，这里的ctrlbuffer是控制点，例如直线的端点，椭圆的中心等*/

		static void translate(
			int x, int y, std::vector<QPoint>& ctrlbuffer
		);
		/*将ctrlbuffer中的点以(x,y)为中心顺时针旋转角度r，这里的ctrlbuffer是控制点，例如直线的端点，椭圆的中心等*/
		static void rotate(
			int x, int y, int r, std::vector<QPoint>& ctrlbuffer
		);
		/*将ctrlbuffer中的点以(x,y)为中心放缩s，这里的ctrlbuffer是控制点，例如直线的端点，椭圆的中心等*/
		static void scale(
			int x, int y, float sx, float sy, std::vector<QPoint>& ctrlbuffer
		);
	private:
		/*求交，没有交点返回(ERRORINT,ERRORINT)*/
		static QPoint getIntersection(QPoint a, double m, short code,
			int xmin, int ymin, int xmax, int ymax);
		/*递归地求n阶曲线*/
		static double bspline(double* U, double u, int i, int k);
	};

	class Shape {
	public:
		enum ShapeType { line = 0, polygon, ellipse, curve } sType;
		enum AlgorithmType { bresenham = 0, dda, bezier, bspline, cohen_sutherland, liang_barsky }aType;
		Shape();
		//  约定控制点的表达
		//  直线：size=2,包含两个端点
		//  椭圆：size=2，包含（x0,y0）（rx,ry）
		std::vector<QPoint> ctrlp;
		std::vector<QPoint> _ctrlp; double angle = 0;
		std::vector<QPoint> buffer;
		QRgb color;
		int id;
		QRect boundingRect;
		bool isModified;
		//  判断鼠标是否选中这个图元
		bool contains(const QPoint& point);
		//  图元编辑操作，调用class Proc中的函数更新控制点，调用filbuffer更新缓冲区
		void translate(int x, int y);
		void rotate(int x, int y, int r);
		void scale(int x, int y, float sx, float sy);
		void clip(int x1, int y1, int x2, int y2, AlgorithmType _atype);
		void render(QImage& img);
	private:
		void fillBuffer();
	};

};
