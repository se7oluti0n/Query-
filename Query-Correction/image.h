#pragma once

//差分の画素の属性
enum DiffType{NON,BACK,NEITHER,SKIN,HAIR,FACE,HAND};

class image {
private:
	
	int Width;              //画像の幅
	int Height;             //画像の高さ
	unsigned char *pixel;   //画像データ(RGB)
	unsigned char *pixel_Y; //輝度値
	double *pixels_Y_d;

	int *p;           //各画素のラベル番号
	int *nlabel;	  //各ラベルの画素数
	int *xcount;      //各ラベルの重心計算用
	int *ycount;      //各ラベルの重心計算用
	int label;        //検出された物体のラベル数+1(添え字とラベル番号は同じ)

	int *xmin;        //人(手)の外接長方刑のx座標の最小値
	int *ymin;        //人(手)の外接長方刑のy座標の最小値
	int *xmax;        //人(手)の外接長方刑のx座標の最大値
	int *ymax;        //人(手)の外接長方刑のy座標の最大値

	int h_label;      //体と離れた手のラベル数
	
	int fxmin[5];     //人の顔の外接長方形のｘ座標の最小値
	int fymin[5];     //人の顔の外接長方形のｙ座標の最小値
	int fxmax[5];     //人の顔の外接長方形のｘ座標の最大値
	int fymax[5];     //人の顔の外接長方形のｙ座標の最大値
	int Person_num;   //画像中の人の数

	bool voice;       //音声入力があるかどうか
	bool Finger;      //指さしているかどうか
	bool pf;          //人フラグ

	int *skin_num;    //ラベル付けされた領域の肌色の数

	bool track_flag;

	int xmin_point;   //指差す領域のx座標の最小値
	int ymin_point;   //指差す領域のy座標の最小値
	int xmax_point;   //指差す領域のx座標の最大値
	int ymax_point;   //指差す領域のy座標の最大値
	
public:
	
	image(){
		Width  = 320;
		Height = 240;
		init();
	}
	image(const int w, const int h){
		Width  = w;
		Height = h;
		init();
	}
	~image(){ /*release();*/ }
	
	/* 画像データサイズの確保 */
	void init(){
		pixel      = new unsigned char[Width*Height*3];
		pixel_Y    = new unsigned char[Width*Height*3];
		pixels_Y_d = new double[Width*Height*3];
	}

	/* 画像データサイズの解放 */
	void pix_reset(void){ pixel=0; }
	void release(void){ 
		if(pixel)      delete [] pixel; 
		if(pixel_Y)    delete [] pixel_Y;
		if(pixels_Y_d) delete [] pixels_Y_d;
	}
	
	/* RGB */
	unsigned char &R(int x,int y){ return pixel[(y*Width+x)*3+2]; }
	unsigned char &G(int x,int y){ return pixel[(y*Width+x)*3+1]; }
	unsigned char &B(int x,int y){ return pixel[(y*Width+x)*3]; }

	/* 輝度値 */
	unsigned char &Y(int x,int y){ return pixel_Y[(y*Width+x)*3+2]; }

	//差分の属性
	unsigned char &isDiff(const int x,const int y){ return pixel_Y[(y*Width+x)*3]; }

	//誤差
	double &E(const int x,const int y  ){ return pixels_Y_d[(y*Width+x)*3  ];}
	double &Y_d(const int x,const int y){ return pixels_Y_d[(y*Width+x)*3+2];}

	/* 監視 */
	int watch(int frame);

	/* 物体検知(変化があったもの) */
	void BackDiff(image &back,image &input);  //背景差分
	void opening();         //オープニング
	void closing();         //クロージング

	/* 人検出 */
	bool Check_User();            //人の検出を行う
	bool Check_Skin();            //顔、指先を求める
	bool labeling(int Xmin,int Xmax,int Ymin,int Ymax);  //ラベリング
	void skincolor();          //肌色検出(YCbCr)
	bool DetectFace(int k);    //OpenCVによる顔(or上半身)検出
	bool Tracking(int k);      //追跡

	int labeling_hand(int Xmin,int Xmax,int Ymin,int Ymax); //肌色のラベリング

	void M_estimator(image &pre,image &now); //Ｍ推定
	int PointDistance(const int a,const int b,const int c,const int d);

	/* 導入課題 */
	//bool blackWhite();    //2値化
	//bool edgedetection(); //エッジ検出

	bool Get_Area(int xmin[],int ymin[],int xmax[],int ymax[]);   //指差す領域を返す
	void set_voice(){voice = true;};

	//デバック用
	void D_skincolor();  //肌色検出
};
