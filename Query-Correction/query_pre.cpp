//---------------------------------------------------
// 内容：検索を行う
// 
// 作成者：牧和宏
// 更新日：2007/02/07
//---------------------------------------------------
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "query.h"
using namespace std;

//---------------------------------------------------------------
// name:Reference(int xmin,int ymin,int xmax,int ymax,int task)
// 指さしている物体の検索
// xmin,ymin,xmax,ymax：指さしている領域
// task:4～6なら放置物、7～9なら紛失物
//
//---------------------------------------------------------------
int Cquery::Reference(int xmin,int ymin,int xmax,int ymax,int task)
{
	char buff[80];    //logファイルの1行分
	ifstream file;    //logファイルオープン用

	//検索タスクの決定
	if(task>=4 && task<=6){ //放置物
		taskflag=LEAVE;
	}else if(task>=7 && task<=9){ //紛失物
		taskflag=MISSING;
	}

	//logファイルのオープン
	//file.open("\\\\Proliant2/Maki/Detected-Object/object.log");
	file.open("./B3-desk/object.log");

	//初期化
	m_num=0;  l_num=0;


	//物体検索
	//とりあえず、指差し領域に物体の重心があるかどうか
	while(file.getline(buff,80)>0){
		SplitData(buff);
		if((xmin-10<=info[0].place.cx) && (info[0].place.cx<=xmax+10)
			&& (ymin-10<=info[0].place.cy) && (info[0].place.cy<=ymax+10)){
				Check_Priority(); //優先度を調べて、格納
			}
	}

	//既にないものは放置物の候補からはずす
	if(taskflag==LEAVE){
		int temp=l_num;
		int blank=-1;
		for(int i=0;i<temp;i++){
			for(int j=0;j<m_num;j++){
				if(LeaveInfo[i].LID==MissingInfo[j].MID){
					if(blank==-1)	blank=i;
					l_num--;
					break; 
				}else{
					if(blank>=0 && j==m_num-1)	LeaveInfo[blank++] = LeaveInfo[i];
				}
			}
		}
	}

	//試しに出力
	/*for(int i=0;i<l_num;i++){
		cout << i << ":" << LeaveInfo[i].frame << '\n';
	}*/

	file.close();


	if(taskflag==LEAVE){
		return l_num;
	}else if(taskflag==MISSING){
		return m_num;
	}
	return 0;
}

//------------------------------------------------
// name:SplitData(char *string)
// 読み込んだ文字列を各データに分ける
// string：入力文字列
//
//-------------------------------------------------
int Cquery::SplitData(char *string)
{
	int flag=0;
	int j=0;
	char value[15];

	for(int i=0;i<(int)strlen(string);i++){
		if(string[i]==','){ //','が来たら
			value[j]='\0';
			j=0;
			switch(flag){
				case 0:	info[0].frame=atoi(value); //フォルダの格納
						break;
				case 1: info[0].place.cx=atoi(value); //重心のx座標格納
						break;
				case 2:	info[0].place.cy=atoi(value); //重心のy座標格納
						break;
				case 3: info[0].place.xmin=atoi(value); //外接長方形(xmin)の格納
						break;
				case 4: info[0].place.ymin=atoi(value); //外接長方形(ymin)の格納
						break;
				case 5: info[0].place.xmax=atoi(value); //外接長方形(xmax)の格納
						break;
				case 6: info[0].place.ymax=atoi(value); //外接長方形(ymax)の格納
						break;
				case 7: info[0].area=atoi(value);  //面積の格納
						break;
				case 9: info[0].MID=atoi(value);   //持ち去りID
						break;
				case 10: info[0].LID=atoi(value);  //持ち込みID
						break;
			}
			flag++;
		}else if(flag<=7 || flag>8){
			value[j++]=string[i];
		}else if(flag==8){
			SplitTime(&string[i]); //時刻格納
			j=0;
			i+=10;
			flag++;
		}
	}
	value[j]='\0';
	info[0].shadow=atoi(value); //影の格納

	return flag;
}

//------------------------------------------------
// name:SplitTime(char *string)
// 読み込んだ文字列を各データ(時間)に分ける
// string：入力文字列
// 時間：月日時分秒
//
//-------------------------------------------------
int Cquery::SplitTime(char *string2)
{
	int flag=0;
	int j=0;
	char value[3];
	int tmp=0;

	for(int i=0;i<10;i++){
		if(j==2){
			value[j]='\0';
			j=0;
			switch(flag){
				case 0: info[0].time.month=atoi(value);  //月の格納
						break;
				case 1: info[0].time.day=atoi(value);    //日の格納
						break;
				case 2: info[0].time.hour=atoi(value);   //時の格納
						break;
				case 3: info[0].time.minute=atoi(value); //分の格納
						break;
			}
			i--;
			tmp=0;
			flag++;
		}else{
			value[j++]=string2[i];
		}
	}
	info[0].time.second=tmp; //秒の格納

	return flag;
}

//---------------------------------------
// name:Check_Priority()
// 優先度のチェックをして、データを格納
// 今は影かどうかだけ
//
//--------------------------------------
void Cquery::Check_Priority()
{
	static int below_lnum=0;  //影ではない放置物データの最も優先度の低いものを指す
	static int below_mnum=0;  //影ではない紛失物データの最も優先度の低いものを指す
	int flag;     //読み込んだデータのシーン

	//初期化(別の検索)
	if(l_num+m_num==0){
		below_lnum=0;
		below_mnum=0;
	}

	//シーンの判断
	if(info[0].MID<0)	flag=LEAVE;
	else				flag=MISSING;

	//格納
	if(info[0].shadow==0){ //影じゃない
		if(flag==LEAVE)	LeaveInfo[l_num++]=info[0];
		else			MissingInfo[m_num++]=info[0];
	}else{ //影
		//入れ替え
		if(flag==LEAVE){
			for(int i=l_num;i>=below_lnum;i--){
				LeaveInfo[i+1] = LeaveInfo[i];
			}
			LeaveInfo[below_lnum++]=info[0];
			l_num++;
		}else{
			for(int i=m_num;i>=below_mnum;i--){
				MissingInfo[i+1] = MissingInfo[i];
			}
			MissingInfo[below_mnum++]=info[0];
			m_num++;
		}
	}
}

//-----------------------------------------
// name:Give_Value()
// 優先度が最も高い要素を返す
//
//-----------------------------------------
int Cquery::Give_Value()
{
	if(taskflag==MISSING)		return MissingInfo[m_num-1].frame;
	else if(taskflag==LEAVE)	return LeaveInfo[l_num-1].frame;

	return 0;
}

//----------------------------------------
// name:Renewal()
// 検索結果を更新する
//
//----------------------------------------
void Cquery::Renewal()
{
	if(taskflag==LEAVE)			l_num--;
	else if(taskflag==MISSING)	m_num--;
}

//-----------------------------------------
// name:Give_Hour()
// 検知時間(時)を返す
//
//-----------------------------------------
int Cquery::Give_Hour()
{
	int num=0;

	if(taskflag==LEAVE){
		static int total=l_num;

		if(total<=0)	total=l_num;
		num = LeaveInfo[total-1].time.hour;
		total--;

		return num;
	}else if(taskflag==MISSING){
		static int total=m_num;

		if(total<=0)	total=m_num;
		num = MissingInfo[total-1].time.hour;
		total--;

		return num;
	}

	return 0;
}

//-----------------------------------------
// name:Give_Hour()
// 検知時間(分)を返す
//
//-----------------------------------------
int Cquery::Give_Minute()
{
	int num=0;

	if(taskflag==LEAVE){
		static int total=l_num;

		if(total<=0)	total=l_num;
		num = LeaveInfo[total-1].time.minute;
		total--;
		
		return num;
	}else if(taskflag==MISSING){
		static int total=m_num;

		if(total<=0)	total=m_num;
		num = MissingInfo[total-1].time.minute;
		total--;
		
		return num;
	}

	return 0;
}

//-----------------------------------------
// name:NarrowByTime(int t)
// 時間により検索を絞る
// t:時間
//
//-----------------------------------------
int Cquery::NarrowByTime(int t)
{
	int tmp_mnum=0;

	//時間で絞る
	if(taskflag==MISSING){
		for(int i=0;i<m_num;i++){
			if((MissingInfo[i].time.hour<t-1) || (MissingInfo[i].time.hour==t-1 && MissingInfo[i].time
				.minute<30)){
				}else{
					MissingInfo[tmp_mnum]=MissingInfo[i];
					tmp_mnum++;
				}
		}
	}

	//ソート
	for(int i=0;i<tmp_mnum/2;i++){
		info[0] = MissingInfo[i];
		MissingInfo[i] = MissingInfo[tmp_mnum-1-i];
		MissingInfo[tmp_mnum-1-i] = info[0];
	}

	m_num=tmp_mnum;

	return m_num;
}



//--------------------------------
// name:InitQuery
// 初期化
//
//-------------------------------
void Cquery::InitQuery()
{
	//どうするか後で考える
	/*for(int i=0;i<l_num;i++){
		LeaveInfo[i].area=0; LeaveInfo[i].frame=0;
		LeaveInfo[i].
	}
	for(int i=0;i<m_num;i++){
		MissingInfo[i]=0;
	}*/
	m_num=0; l_num=0; taskflag=0;
}