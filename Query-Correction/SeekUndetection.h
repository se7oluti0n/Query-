#pragma once

#include "cv.h"
#include "highgui.h"

class CseekUndetection
{
private:
	int xmin;  //’Tõ—Ìˆæ
	int ymin;
	int xmax;
	int ymax;
	IplImage *back,*bgray;
public:
	CseekUndetection(){Init();}
	void Init();
	int Reference(int st,int ed);

	void BackDiff(IplImage *input,IplImage *human);

	void Set_Area(int x1,int x2,int y1,int y2){xmin=x1;xmax=x2;ymin=y1;ymax=y2;}
	int Get_Area(){return xmax+ymax;}
};