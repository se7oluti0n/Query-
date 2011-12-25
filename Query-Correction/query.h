#pragma once

//場所情報
typedef struct _Place{
	int cx;    //重心のx座標
	int cy;    //重心のy座標
	int xmin;
	int xmax;
	int ymin;
	int ymax;
} Place;

//時間情報
typedef struct _Time{
	int month;
	int day;
	int hour;
	int minute;
	int second;
} Time;

//物体情報
typedef struct _Object{
	int frame;    //フォルダ
	Place place;  //場所
	int area;     //面積
	Time time;    //時間
	int MID;      //持ち去りID
	int LID;      //持ち込みID
	int shadow;   //影かどうか
} Object;

enum Scene{MISSING,LEAVE};

class Cquery{
private:
	Object info[50];
	Object LeaveInfo[30];    //放置物
	Object MissingInfo[30];  //紛失物
	Object LeftInfo[30];     //既に無いが置かれた物

	int m_num;     //紛失物の候補数
	int l_num;     //放置物の候補数
	int taskflag;  //実行タスク

	int left_num;

	int total_obj_num; //イベント総数

public:
	void InitQuery(); //初期化
	int SplitData(char *string);  //データを分ける
	int SplitTime(char *string2); //時間を分ける
	int Reference(int xmin,int ymin,int xmax,int ymax,int task); //検索

	void Check_Priority();   //優先度を調べて、格納
	int NarrowByTime(int t); //時間で絞る
	void Renewal();          //検索結果を更新する 

	int Give_Value();        //優先度が最も高い要素を返す

	//時間を返す++++++++++++++++++++++
	int Give_Hour();
	int Give_Minute();
	//++++++++++++++++++++++++++++++++

	int Get_lnum(){return l_num;}
	int Get_leftnum(){return left_num;}
	int Get_mnum(){return m_num;}

	Object* Get_lnum_info(){return LeaveInfo;}
	Object* Get_ltnum_info(){return LeftInfo;}
	Object* Get_mnum_info(){return MissingInfo;}

	Object* Get_total_info(){return info;}
	int Get_total_num(){return total_obj_num;}
};