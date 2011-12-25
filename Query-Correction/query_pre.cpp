//---------------------------------------------------
// ���e�F�������s��
// 
// �쐬�ҁF�q�a�G
// �X�V���F2007/02/07
//---------------------------------------------------
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "query.h"
using namespace std;

//---------------------------------------------------------------
// name:Reference(int xmin,int ymin,int xmax,int ymax,int task)
// �w�����Ă��镨�̂̌���
// xmin,ymin,xmax,ymax�F�w�����Ă���̈�
// task:4�`6�Ȃ���u���A7�`9�Ȃ畴����
//
//---------------------------------------------------------------
int Cquery::Reference(int xmin,int ymin,int xmax,int ymax,int task)
{
	char buff[80];    //log�t�@�C����1�s��
	ifstream file;    //log�t�@�C���I�[�v���p

	//�����^�X�N�̌���
	if(task>=4 && task<=6){ //���u��
		taskflag=LEAVE;
	}else if(task>=7 && task<=9){ //������
		taskflag=MISSING;
	}

	//log�t�@�C���̃I�[�v��
	//file.open("\\\\Proliant2/Maki/Detected-Object/object.log");
	file.open("./B3-desk/object.log");

	//������
	m_num=0;  l_num=0;


	//���̌���
	//�Ƃ肠�����A�w�����̈�ɕ��̂̏d�S�����邩�ǂ���
	while(file.getline(buff,80)>0){
		SplitData(buff);
		if((xmin-10<=info[0].place.cx) && (info[0].place.cx<=xmax+10)
			&& (ymin-10<=info[0].place.cy) && (info[0].place.cy<=ymax+10)){
				Check_Priority(); //�D��x�𒲂ׂāA�i�[
			}
	}

	//���ɂȂ����͕̂��u���̌�₩��͂���
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

	//�����ɏo��
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
// �ǂݍ��񂾕�������e�f�[�^�ɕ�����
// string�F���͕�����
//
//-------------------------------------------------
int Cquery::SplitData(char *string)
{
	int flag=0;
	int j=0;
	char value[15];

	for(int i=0;i<(int)strlen(string);i++){
		if(string[i]==','){ //','��������
			value[j]='\0';
			j=0;
			switch(flag){
				case 0:	info[0].frame=atoi(value); //�t�H���_�̊i�[
						break;
				case 1: info[0].place.cx=atoi(value); //�d�S��x���W�i�[
						break;
				case 2:	info[0].place.cy=atoi(value); //�d�S��y���W�i�[
						break;
				case 3: info[0].place.xmin=atoi(value); //�O�ڒ����`(xmin)�̊i�[
						break;
				case 4: info[0].place.ymin=atoi(value); //�O�ڒ����`(ymin)�̊i�[
						break;
				case 5: info[0].place.xmax=atoi(value); //�O�ڒ����`(xmax)�̊i�[
						break;
				case 6: info[0].place.ymax=atoi(value); //�O�ڒ����`(ymax)�̊i�[
						break;
				case 7: info[0].area=atoi(value);  //�ʐς̊i�[
						break;
				case 9: info[0].MID=atoi(value);   //��������ID
						break;
				case 10: info[0].LID=atoi(value);  //��������ID
						break;
			}
			flag++;
		}else if(flag<=7 || flag>8){
			value[j++]=string[i];
		}else if(flag==8){
			SplitTime(&string[i]); //�����i�[
			j=0;
			i+=10;
			flag++;
		}
	}
	value[j]='\0';
	info[0].shadow=atoi(value); //�e�̊i�[

	return flag;
}

//------------------------------------------------
// name:SplitTime(char *string)
// �ǂݍ��񂾕�������e�f�[�^(����)�ɕ�����
// string�F���͕�����
// ���ԁF���������b
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
				case 0: info[0].time.month=atoi(value);  //���̊i�[
						break;
				case 1: info[0].time.day=atoi(value);    //���̊i�[
						break;
				case 2: info[0].time.hour=atoi(value);   //���̊i�[
						break;
				case 3: info[0].time.minute=atoi(value); //���̊i�[
						break;
			}
			i--;
			tmp=0;
			flag++;
		}else{
			value[j++]=string2[i];
		}
	}
	info[0].time.second=tmp; //�b�̊i�[

	return flag;
}

//---------------------------------------
// name:Check_Priority()
// �D��x�̃`�F�b�N�����āA�f�[�^���i�[
// ���͉e���ǂ�������
//
//--------------------------------------
void Cquery::Check_Priority()
{
	static int below_lnum=0;  //�e�ł͂Ȃ����u���f�[�^�̍ł��D��x�̒Ⴂ���̂��w��
	static int below_mnum=0;  //�e�ł͂Ȃ��������f�[�^�̍ł��D��x�̒Ⴂ���̂��w��
	int flag;     //�ǂݍ��񂾃f�[�^�̃V�[��

	//������(�ʂ̌���)
	if(l_num+m_num==0){
		below_lnum=0;
		below_mnum=0;
	}

	//�V�[���̔��f
	if(info[0].MID<0)	flag=LEAVE;
	else				flag=MISSING;

	//�i�[
	if(info[0].shadow==0){ //�e����Ȃ�
		if(flag==LEAVE)	LeaveInfo[l_num++]=info[0];
		else			MissingInfo[m_num++]=info[0];
	}else{ //�e
		//����ւ�
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
// �D��x���ł������v�f��Ԃ�
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
// �������ʂ��X�V����
//
//----------------------------------------
void Cquery::Renewal()
{
	if(taskflag==LEAVE)			l_num--;
	else if(taskflag==MISSING)	m_num--;
}

//-----------------------------------------
// name:Give_Hour()
// ���m����(��)��Ԃ�
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
// ���m����(��)��Ԃ�
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
// ���Ԃɂ�茟�����i��
// t:����
//
//-----------------------------------------
int Cquery::NarrowByTime(int t)
{
	int tmp_mnum=0;

	//���Ԃōi��
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

	//�\�[�g
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
// ������
//
//-------------------------------
void Cquery::InitQuery()
{
	//�ǂ����邩��ōl����
	/*for(int i=0;i<l_num;i++){
		LeaveInfo[i].area=0; LeaveInfo[i].frame=0;
		LeaveInfo[i].
	}
	for(int i=0;i<m_num;i++){
		MissingInfo[i]=0;
	}*/
	m_num=0; l_num=0; taskflag=0;
}