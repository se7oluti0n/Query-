#pragma once

//�ꏊ���
typedef struct _Place{
	int cx;    //�d�S��x���W
	int cy;    //�d�S��y���W
	int xmin;
	int xmax;
	int ymin;
	int ymax;
} Place;

//���ԏ��
typedef struct _Time{
	int month;
	int day;
	int hour;
	int minute;
	int second;
} Time;

//���̏��
typedef struct _Object{
	int frame;    //�t�H���_
	Place place;  //�ꏊ
	int area;     //�ʐ�
	Time time;    //����
	int MID;      //��������ID
	int LID;      //��������ID
	int shadow;   //�e���ǂ���
} Object;

enum Scene{MISSING,LEAVE};

class Cquery{
private:
	Object info[50];
	Object LeaveInfo[30];    //���u��
	Object MissingInfo[30];  //������
	Object LeftInfo[30];     //���ɖ������u���ꂽ��

	int m_num;     //�������̌�␔
	int l_num;     //���u���̌�␔
	int taskflag;  //���s�^�X�N

	int left_num;

	int total_obj_num; //�C�x���g����

public:
	void InitQuery(); //������
	int SplitData(char *string);  //�f�[�^�𕪂���
	int SplitTime(char *string2); //���Ԃ𕪂���
	int Reference(int xmin,int ymin,int xmax,int ymax,int task); //����

	void Check_Priority();   //�D��x�𒲂ׂāA�i�[
	int NarrowByTime(int t); //���Ԃōi��
	void Renewal();          //�������ʂ��X�V���� 

	int Give_Value();        //�D��x���ł������v�f��Ԃ�

	//���Ԃ�Ԃ�++++++++++++++++++++++
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