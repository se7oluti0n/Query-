#include "stdafx.h"
#include <iostream>
using namespace std;
#include "QueryHuman.h"
#include <fstream>

int CqueryHuman::Reference()
{
	char buff[30];
	ifstream file;

	file.open("./Human-DB/human.log");

	//初期化
	num=0;

	while(file.getline(buff,30)>0){
		SplitData(buff);
		num++;
	}
	if(num>10)	num=10;

	//試しに出力
	for(int i=0;i<num;i++){
		cout << Info[i].time.hour << ":" << Info[i].time.minute << ":" 
			<< Info[i].time.second << "検知\n";
		cout << Info[i].sframe << "〜" << Info[i].eframe << "フレーム\n\n";
	}

	return num;
}

//------------------------------------------------
// name:SplitData(char *string)
// 読み込んだ文字列を各データに分ける
// string：入力文字列
//
//-------------------------------------------------
int CqueryHuman::SplitData(char *string)
{
	int flag=0;
	int j=0;
	char value[15];

	//入れ替え
	for(int i=0;i<9;i++)
		Info[10-i-1] = Info[10-i-2];

	for(int i=0;i<(int)strlen(string);i++){
		if(string[i]==','){ //','が来たら
			value[j]='\0';
			j=0;
			if(flag==0){
				Info[0].sframe=atoi(value); //開始フレーム格納
			}
			flag++;
		}else if(flag<=0 || flag>1){
			value[j++]=string[i];
		}else if(flag==1){
			SplitTime(&string[i]); //時刻格納
			j=0;
			i+=6;
			flag++;
		}
	}
	value[j]='\0';
	Info[0].eframe=atoi(value);
	Info[0].check = false;

	return flag;
}

//------------------------------------------------
// name:SplitTime(char *string)
// 読み込んだ文字列を各データ(時間)に分ける
// string：入力文字列
// 時間：時分秒
//
//-------------------------------------------------
int CqueryHuman::SplitTime(char *string2)
{
	int flag=0;
	int j=0;
	char value[3];
	int tmp=0;

	for(int i=0;i<6;i++){
		if(j==2){
			value[j]='\0';
			j=0;
			switch(flag){
				case 0: Info[0].time.hour=atoi(value); //時の格納
						break;
				case 1: Info[0].time.minute=atoi(value); //分の格納
						break;
			}
			i--;
			tmp=0;
			flag++;
		}else{
			value[j++]=string2[i];
		}
	}
	value[j]='\0';
	Info[0].time.second = atoi(value);  //秒の格納

	return flag;
}