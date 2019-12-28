#include "cmd.h"
#include <map>
#include <iostream>

using namespace std;

namespace cgcore {
	static map<string, int> cmdTable = {
		/*重置画布：
				resetCanvas width height
				清空当前画布，并重新设置宽高
				width, height: int
				100 <= width, height <= 1000*/
		{"resetcanvas",RESETCANVAS},
		/*保存画布：
				saveCanvas name
				将当前画布保存为位图name.bmp
				name: string*/
		{"savecanvas",SAVECANVAS},
		/*设置画笔颜色：
				setColor R G B
				R, G, B: int
				0 <= R, G, B <= 255*/
		{"setcolor",SETCOLOR},
		/*绘制线段：
				drawLine id x1 y1 x2 y2 algorithm
				id: int
				图元编号，每个图元的编号是唯一的
				x1, y1, x2, y2: int
				起点、终点坐标
				algorithm: string
				绘制使用的算法，包括“DDA”和“Bresenham”*/
		{"drawline",DRAWLINE},
		/*绘制多边形：
				drawPolygon id n algorithm
				x1 y1 x2 y2 … xn yn
				id: int
				图元编号，每个图元的编号是唯一的
				n: int
				顶点数
				x1, y1, x2, y2 ... : int
				顶点坐标
				algorithm: string
				绘制使用的算法，包括“DDA”和“Bresenham”*/
		{"drawpolygon",DRAWPOLYGON},
		/*绘制椭圆（中点圆生成算法）：
				drawEllipse id x y rx ry
				id: int
				图元编号，每个图元的编号是唯一的
				x, y: int
				圆心坐标
				rx, ry: int
				长短轴半径*/
		{"drawellipse",DRAWELLIPSE},
		/*绘制曲线：
				drawCurve id n algorithm
				x1 y1 x2 y2 … xn yn
				id: int
				图元编号，每个图元的编号是唯一的
				n: int
				控制点数量
				x1, y1, x2, y2 ... : int
				控制点坐标
				algorithm: string
				绘制使用的算法，包括“Bezier”和“B-spline”*/
		{"drawcurve",DRAWCURVE},
		/*对图元平移：
				translate id dx dy
				id: int
				要平移的图元编号
				dx, dy: int
				平移向量*/
		{"translate",TRANSLATE},
		/*对图元旋转：
				rotate id x y r
				id: int
				要旋转的图元编号
				x, y: int
				旋转中心
				r: int
				顺时针旋转角度（°）*/
		{"rotate",ROTATE},
		/*对图元缩放：
				scale id x y s
				id: int
				要缩放的图元编号
				x, y: int
				缩放中心
				s: float
				缩放倍数*/
		{"scale",SCALE},
		/*对线段裁剪：
				clip id x1 y1 x2 y2 algorithm
				id: int
				要裁剪的图元编号
				x1, y1, x2, y2: int
				裁剪窗口左下、右上角坐标
				algorithm: string
				裁剪使用的算法，包括“Cohen-Sutherland”和“Liang-Barsky”*/
		{"clip",CLIP}
	};

	static map<string, int> algorithmTable = {
		{"dda",DDA},
		{"bresenham",BRESENHAM},
		{"bezier",BEZIER},
		{"b-spline",B_SPLINE},
		{"cohen-sutherland",COHEN_SUTHERLAND},
		{"liang-barsky",LIANG_BARSKY}
	};

	int cmd2cmd(const string& _cmd) {
		string cmd;
		for (auto i : _cmd) {
			cmd.push_back(tolower(i));
		}
		map<string, int>::iterator iter = cmdTable.find(cmd);
		if (iter == cmdTable.end())
			return NOTFOUND;
		return iter->second;
	}

	int cmd2algorithm(const string& _cmd) {
		string cmd;
		for (auto i : _cmd) {
			cmd.push_back(tolower(i));
		}
		map<string, int>::iterator iter = algorithmTable.find(cmd);
		if (iter == cmdTable.end())
			return NOTFOUND;
		return iter->second;
	}

};
