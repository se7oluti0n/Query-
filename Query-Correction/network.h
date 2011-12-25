#pragma once

#include "bmp.h"
#include "QueryByMouse.h"
#include "QueryByMouseDlg.h"
#include <WinSock2.h>
#include "cv.h"
class CQueryByMouseDlg;

//-----------------
// Class
//-----------------
class CCamera : public image{
private:
	int Width;          //JPEG画像データの幅
	int Height;         //JPEG画像データの高さ
	unsigned char *nR;  //赤の要素のビットデータ
	unsigned char *nG;  //緑の画素のビットデータ
	unsigned char *nB;  //青の画素のビットデータ
	int stop_flag;      //動画像停止フラグ
	
	int direction;      //変更するカメラの向き
	CQueryByMouseDlg* pParent;

	bmp input;  //オフライン用BMPクラス

	//cv::Mat outt;
	int isPointing, minx, miny, maxx, maxy;
	SOCKET sock_for_finger; // Socket for detect pointing

public:
	
	int SendCommand(int camera,int task);  //文字列を送る
	int OffLine();                         //オフライン用BMPファイルの読み込み

	void set_stop(){ stop_flag=1; }        //動画像停止フラグをONにする
	
	void ChangeDirection(int course){ direction = course; }  //変更するカメラの向きをセット
	
	void set(CQueryByMouseDlg* parent){pParent=parent;}
	bool get_finger(int xmin[],int ymin[],int xmax[],int ymax[]); //指さしている領域を返す

	void set_pParent(CQueryByMouseDlg* p){pParent = p;} //親ウィンドウへのポインタ設定
	//bool get_finger(int xmin[],int ymin[],int xmax[],int ymax[]); //指さしている領域を返す
	int imageKinect(CCamera * cp);

};