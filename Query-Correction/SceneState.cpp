#include "stdafx.h"
#include ".\scenestate.h"
#include <iostream>
using namespace std;

//#pragma warning(disable: 4244)

//-------------------------
//constructor
//-------------------------
Scene::Scene(void)
{

}

//-------------------------
//destructor
//-------------------------
Scene::~Scene(void)
{
}

//------------------------------------------------------------------------
// Name     : detection(bmp &now,bmp &pre,Area &area)
// Author�@ : Katayama Noriaki (CV-lab.)
// function : ���̗̈�̌`���ߋ��Ɣ�ׂ�
// Argument : bmp  ���͉摜
//            bmp  �ߋ��̓��͉摜
// return   : ���̌��m��1��Ԃ�
// Update   : 2006.6.18
//------------------------------------------------------------------------
int Scene::detection(IplImage *now,IplImage *pre)
{
	int same_pix = 0;//�����ꏊ�̉�f
	int now_num  = 0; //���݂̍����̐�

	//############################################
	//���ڂ������x���̗̈�ɂ���
	//���̗̈�ɉߋ��ɂ������傫�����炢�̓������x����
	//�������̂����������̂����ׂ�
	//����΁@�{�P�@������΁@�|�P

	//�P�O�̃t���[���ƕ��̗̈�ɂ��ĉ�f�̐��𒲂ׂ�
	for(int j = ymin; j < ymax; j++){
		for(int i = xmin; i < xmax; i++){
			uchar no=now->imageData[(now->widthStep*j+i*3)];
			uchar po=pre->imageData[(pre->widthStep*j+i*3)];
			if(no==255)
				now_num++;
			if(no==255 && po!=255)
				same_pix--;
			if(no==255 && po==255)
				same_pix++;
		}
	}

	if(now_num < 70 || same_pix < 0) return 0;  

	double hold = (double)now_num * 0.7;

	//臒l�ȓ��Ȃ畨�̌��m�������Ƃɂ���
	if(same_pix > hold){ return 1; }

	//������Ȃ�������
	return 0;
}

//------------------------------------------------------------------------
// Name     : check(bmp &in)
// Author�@ : Katayama Noriaki (CV-lab.)
// function : ���̂����m���邽�߂Ɉ��t���[���Ď�
// Argument : bmp ���̉摜
// return   : bool ���t���[���̓����ʒu�ɕ��̗̈悪�����true
// Update   : 2006.7.12
//------------------------------------------------------------------------
bool Scene::check(IplImage *in)
{
	//IplImage* input;//���͉摜�̃R�s�[
	static bool obj_flag;
	int count = 0; //���̂��ϑ������V�[���̐�

	index++;

	if(index==1 || img_t1==0){
		img_t1=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t2=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t3=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t4=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t5=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t6=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
	}

	//���͉摜���R�s�[
	cvCopy(in,img_t6,0);

	//���݂̓��͉摜���珇�Ԃɉߋ��̗��������Ă���
	if(index > 5){

		//count += detection(*tail,*img_t8);
		//count += detection(*tail,*img_t7);
		//count += detection(*tail,*img_t6);
		count += detection(img_t6,img_t5);
		count += detection(img_t6,img_t4);
		count += detection(img_t6,img_t3);
		count += detection(img_t6,img_t2);
		count += detection(img_t6,img_t1);
		//count += detection(*tail,*top);

		//���͉摜�Ɖߋ������Ă���
		//�����ȏ�̃t���[���ŕ��̂��m�F���ꂽ�畨�̌��m�Ƃ���
		if(count > 3){
			obj_flag = true;

#if 0//���m�������̂̊O�ڒ����`�𑾂����ň͂�
			int w_size = x2 - x1;
			int h_size = y2 - y1;
			if(w_size < 200 || h_size < 200 ){

				for (int x = x1; x <= x2; x++){
					tail->SetPixColor(x,y1  ,0,255,255);
					tail->SetPixColor(x,y1-1,0,255,255);
					tail->SetPixColor(x,y1+1,0,255,255);
					tail->SetPixColor(x,y2  ,0,255,255);
					tail->SetPixColor(x,y2-1,0,255,255);
					tail->SetPixColor(x,y2+1,0,255,255);
				}
				for (int y = y1; y <= y2; y++){
					tail->SetPixColor(x1  ,y,0,255,255);
					tail->SetPixColor(x1-1,y,0,255,255);
					tail->SetPixColor(x1+1,y,0,255,255);
					tail->SetPixColor(x2  ,y,0,255,255);
					tail->SetPixColor(x2-1,y,0,255,255);
					tail->SetPixColor(x2+1,y,0,255,255);
				}
			}
#endif
		}
		else{
			//���̂𔭌��ł��Ȃ�������
			obj_flag = false;

		}
	}

	//===============================================
	//�P��̏�ԂɑS�Ẵ|�C���^��1�Â��炷
	cvCopy(img_t2,img_t1,0);
	cvCopy(img_t3,img_t2,0);
	cvCopy(img_t4,img_t3,0);
	cvCopy(img_t5,img_t4,0);
	cvCopy(img_t6,img_t5,0);
	//tail   = NULL;

	//������t���[���ŕ��̂𔭌��������H
	if(obj_flag == true )	return true;
	return false;
}
