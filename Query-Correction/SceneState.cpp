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
// Author　 : Katayama Noriaki (CV-lab.)
// function : 物体領域の形を過去と比べる
// Argument : bmp  入力画像
//            bmp  過去の入力画像
// return   : 物体検知で1を返す
// Update   : 2006.6.18
//------------------------------------------------------------------------
int Scene::detection(IplImage *now,IplImage *pre)
{
	int same_pix = 0;//同じ場所の画素
	int now_num  = 0; //現在の差分の数

	//############################################
	//注目したラベルの領域について
	//その領域に過去にも同じ大きさくらいの同じラベルが
	//あったのか無かったのか調べる
	//あれば　＋１　無ければ　－１

	//１つ前のフレームと物体領域について画素の数を調べる
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

	//閾値以内なら物体検知したことにする
	if(same_pix > hold){ return 1; }

	//見つからなかった時
	return 0;
}

//------------------------------------------------------------------------
// Name     : check(bmp &in)
// Author　 : Katayama Noriaki (CV-lab.)
// function : 物体を検知するために一定フレーム監視
// Argument : bmp 物体画像
// return   : bool 一定フレームの同じ位置に物体領域があればtrue
// Update   : 2006.7.12
//------------------------------------------------------------------------
bool Scene::check(IplImage *in)
{
	//IplImage* input;//入力画像のコピー
	static bool obj_flag;
	int count = 0; //物体を観測したシーンの数

	index++;

	if(index==1 || img_t1==0){
		img_t1=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t2=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t3=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t4=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t5=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
		img_t6=cvCreateImage(cvSize(in->width,in->height),IPL_DEPTH_8U,3);
	}

	//入力画像をコピー
	cvCopy(in,img_t6,0);

	//現在の入力画像から順番に過去の履歴を見ていく
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

		//入力画像と過去を見ていき
		//半分以上のフレームで物体が確認されたら物体検知とする
		if(count > 3){
			obj_flag = true;

#if 0//検知した物体の外接長方形を太い線で囲む
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
			//物体を発見できなかったら
			obj_flag = false;

		}
	}

	//===============================================
	//１つ先の状態に全てのポインタを1つづつずらす
	cvCopy(img_t2,img_t1,0);
	cvCopy(img_t3,img_t2,0);
	cvCopy(img_t4,img_t3,0);
	cvCopy(img_t5,img_t4,0);
	cvCopy(img_t6,img_t5,0);
	//tail   = NULL;

	//ある一定フレームで物体を発見したか？
	if(obj_flag == true )	return true;
	return false;
}
