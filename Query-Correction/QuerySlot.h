//---------------------------------------------------
// 内容：問い合わせを行うためのスロット構造の宣言
// 
// 作成者：牧和宏
// 更新日：2007/01/24
//---------------------------------------------------
#pragma once

#include "Flag.h"

class CQuerySlot
{
private:
	//ユーザが示す場所(修正したほうが良い？)++
	int area_xmin;
	int area_xmax;
	int area_ymin;
	int area_ymax;
	//++++++++++++++++++++++++++++++++++++++++

	int CandObjNum;   //物体の候補数
	CFlag Object;     //物体番号(物体が検知されたフレーム番号)、確認が取れたか
	CFlag time;       //時間
	CFlag area;       //場所
	CFlag correction; //訂正モード

public:
	CQuerySlot(void){InitSlot();};  //コンストラクタ
	~CQuerySlot(void){}; //デストラクタ
	void InitSlot();     //スロットの初期化

	//スロットの値を返す+++++++++++++++++++++++++
	int GetArea(){return area_xmin+area_xmax;}
	int GetArea_xmin(){return area_xmin;}
	int GetArea_xmax(){return area_xmax;}
	int GetArea_ymin(){return area_ymin;}
	int GetArea_ymax(){return area_ymax;}
	int GetCandObjNum(){return CandObjNum;}
	int GetObjectNum(){return Object.GetValue();}
	int GetTimeNum(){return time.GetValue();}

	bool GetObjectOK(){return Object.GetOk();}
	bool GetTimeOK(){return time.GetOk();}

	bool GetAreaOK(){return area.GetOk();}
	bool GetCorOK(){return correction.GetOk();}
	int GetCorNum(){return correction.GetValue();}
	//+++++++++++++++++++++++++++++++++++++++++++

	//スロットの値を埋める+++++++++++++++++++++++
	void SetArea_xmin(int x){area_xmin=x;}
	void SetArea_xmax(int x){area_xmax=x;}
	void SetArea_ymin(int y){area_ymin=y;}
	void SetArea_ymax(int y){area_ymax=y;}
	void SetCandObjNum(int n){CandObjNum=n;}
	void SetObjectNum(int n){Object.SetValue(n);}
	void SetTime(int t){time.SetValue(t);}

	void SetObjectOK(bool f){Object.SetOk(f);}
	void SetTimeOK(bool f){time.SetOk(f);}

	void SetAreaOK(bool f){area.SetOk(f);}
	void SetCorOK(bool f){correction.SetOk(f);}
	void SetCorNum(int n){correction.SetValue(n);}
	//+++++++++++++++++++++++++++++++++++++++++++

};