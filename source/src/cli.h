#pragma once
#include "cmd.h"
#include "proc.h"
#include <string>
#include <vector>
#include <QColor>
#include <QImage>
#include <QMap>
#pragma execution_character_set("UTF-8")
using namespace std;

class Cli {
public:
	Cli();
	~Cli();
	bool handleCmd(std::string _cmd = std::string("resetcanvas 100 100"));
	bool handleScript(const char* filename = "");
private:
	QImage rawImg;
	int smdCmd;//  处理几个xx的多行命令 0：dda多边形 1：breshman多边形 2：贝塞尔曲线 3：B样条
	int						//  这是一堆常用变量
		w, h,				//  原始图像长宽
		newW, newH,			//  新图像长宽
		R, G, B, alpha,		//  画笔颜色
		id, n,				//  标签
		angleR;				//  顺时针旋转角度
	std::vector<int> tmpx;	//  临时空间
	QRgb color;
	QMap<int, cgcore::Shape*> shapes;
	void repaintAll();
};

