#pragma once

//時間情報
typedef struct _Time1{
	//int month;
	//int day;
	int hour;
	int minute;
	int second;
} Time1;

//Human-DB情報
typedef struct _Human{
	int sframe;  //開始フレーム
	int eframe;  //終了フレーム
	Time1 time;   //時間
	bool check;  //未検出検索時のアクセスチェック
} Human;

class CqueryHuman
{
private:
	Human Info[10];
	int num;
public:
	int SplitData(char *string);  //データを分ける
	int SplitTime(char *string2); //時間を分ける
	int Reference();

	int Get_sframe(int num){return Info[num].sframe;}
	int Get_eframe(int num){return Info[num].eframe;}
	int Get_hour(int num){return Info[num].time.hour;}
};