#pragma once

//���ԏ��
typedef struct _Time1{
	//int month;
	//int day;
	int hour;
	int minute;
	int second;
} Time1;

//Human-DB���
typedef struct _Human{
	int sframe;  //�J�n�t���[��
	int eframe;  //�I���t���[��
	Time1 time;   //����
	bool check;  //�����o�������̃A�N�Z�X�`�F�b�N
} Human;

class CqueryHuman
{
private:
	Human Info[10];
	int num;
public:
	int SplitData(char *string);  //�f�[�^�𕪂���
	int SplitTime(char *string2); //���Ԃ𕪂���
	int Reference();

	int Get_sframe(int num){return Info[num].sframe;}
	int Get_eframe(int num){return Info[num].eframe;}
	int Get_hour(int num){return Info[num].time.hour;}
};