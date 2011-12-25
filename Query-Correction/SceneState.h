#pragma once
#include "cv.h"
#include "highgui.h"

//----------------------------
//�V�[���̏��
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

	//������
	void init(){
		index=0;
	}

	//���t���[���ɕ��̃��x���̗̈�𔭌��������H
	bool check(IplImage *in);

	//�Q���̉摜���r���ĕ��̂����m�������H
	int detection(IplImage *now,IplImage *pre);

	void SetArea(int x1,int x2,int y1,int y2){xmin=x1; xmax=x2; ymin=y1; ymax=y2;}
};
