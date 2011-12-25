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

	//‰Šú‰»
	num=0;

	while(file.getline(buff,30)>0){
		SplitData(buff);
		num++;
	}
	if(num>10)	num=10;

	//‚µ‚Éo—Í
	for(int i=0;i<num;i++){
		cout << Info[i].time.hour << ":" << Info[i].time.minute << ":" 
			<< Info[i].time.second << "ŒŸ’m\n";
		cout << Info[i].sframe << "`" << Info[i].eframe << "ƒtƒŒ[ƒ€\n\n";
	}

	return num;
}

//------------------------------------------------
// name:SplitData(char *string)
// “Ç‚İ‚ñ‚¾•¶š—ñ‚ğŠeƒf[ƒ^‚É•ª‚¯‚é
// stringF“ü—Í•¶š—ñ
//
//-------------------------------------------------
int CqueryHuman::SplitData(char *string)
{
	int flag=0;
	int j=0;
	char value[15];

	//“ü‚ê‘Ö‚¦
	for(int i=0;i<9;i++)
		Info[10-i-1] = Info[10-i-2];

	for(int i=0;i<(int)strlen(string);i++){
		if(string[i]==','){ //','‚ª—ˆ‚½‚ç
			value[j]='\0';
			j=0;
			if(flag==0){
				Info[0].sframe=atoi(value); //ŠJnƒtƒŒ[ƒ€Ši”[
			}
			flag++;
		}else if(flag<=0 || flag>1){
			value[j++]=string[i];
		}else if(flag==1){
			SplitTime(&string[i]); //Ši”[
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
// “Ç‚İ‚ñ‚¾•¶š—ñ‚ğŠeƒf[ƒ^(ŠÔ)‚É•ª‚¯‚é
// stringF“ü—Í•¶š—ñ
// ŠÔF•ª•b
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
				case 0: Info[0].time.hour=atoi(value); //‚ÌŠi”[
						break;
				case 1: Info[0].time.minute=atoi(value); //•ª‚ÌŠi”[
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
	Info[0].time.second = atoi(value);  //•b‚ÌŠi”[

	return flag;
}