//---------------------------------------------------
// ���e�F�₢���킹���s�����߂̃X���b�g�\���̐錾
// 
// �쐬�ҁF�q�a�G
// �X�V���F2007/01/24
//---------------------------------------------------
#pragma once

#include "Flag.h"

class CQuerySlot
{
private:
	//���[�U�������ꏊ(�C�������ق����ǂ��H)++
	int area_xmin;
	int area_xmax;
	int area_ymin;
	int area_ymax;
	//++++++++++++++++++++++++++++++++++++++++

	int CandObjNum;   //���̂̌�␔
	CFlag Object;     //���̔ԍ�(���̂����m���ꂽ�t���[���ԍ�)�A�m�F����ꂽ��
	CFlag time;       //����
	CFlag area;       //�ꏊ
	CFlag correction; //�������[�h

public:
	CQuerySlot(void){InitSlot();};  //�R���X�g���N�^
	~CQuerySlot(void){}; //�f�X�g���N�^
	void InitSlot();     //�X���b�g�̏�����

	//�X���b�g�̒l��Ԃ�+++++++++++++++++++++++++
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

	//�X���b�g�̒l�𖄂߂�+++++++++++++++++++++++
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