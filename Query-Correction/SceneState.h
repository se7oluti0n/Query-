#pragma once
#include "cv.h"
#include "highgui.h"

//----------------------------
//シーンの状態
//----------------------------
class Scene
{
private:
	IplImage *top,*tail;
	IplImage *img_t1,*img_t2,*img_t3,*img_t4,*img_t5,*img_t6;
	int xmin,xmax,ymin,ymax;
	int index;

public:
	//constructor
	Scene();

	//destructor
	~Scene();

	//初期化
	void init(){
		index=0;
	}

	//一定フレームに物体ラベルの領域を発見したか？
	bool check(IplImage *in);

	//２枚の画像を比較して物体を検知したか？
	int detection(IplImage *now,IplImage *pre);

	void SetArea(int x1,int x2,int y1,int y2){xmin=x1; xmax=x2; ymin=y1; ymax=y2;}
};
