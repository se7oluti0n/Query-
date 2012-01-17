// QueryByMouseDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once

#include "network.h"
#include "bmp.h"

#include "AllocateMemory.h"
#include "constants.h"
#include "query.h"
#include "Output.h"
#include "QuerySlot.h"
#include "TalkedContents.h"

#include "Multi_Disp.h"
#include "afxwin.h"

//�F������
typedef struct _Sentence{
	int ElementsNum;          //�`�ԑf��
	int* CategoryID;          //�J�e�S��ID
	int* WordID;              //�P��ID
	float* ElementConf;       //�`�ԑf�̐M���x
	char** ElementText;       //�`�ԑf�̃e�L�X�g
} mySentence;

// CQueryByMouseDlg �_�C�A���O
class CQueryByMouseDlg : public CDialog
{
private:
	//�\���֌W++++++++++++++++++++++++++++++++++++++++++++++
	BITMAPINFO bmpInfo; //bmp�t�@�C���p
	PAINTSTRUCT ps;

	HWND hwndImage;  //���A���^�C���摜�o�͗p�̈�
	HDC hdcImage;    //�f�o�C�X�R���e�L�X�g�̃n���h��
	
	int pointx;
	int pointy;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//�Θb�֌W++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	int record;   // �Θb�̗���
	int nbest;    // Nbest��N
	mySentence st[MAX_CAND_NUM];   // �F�����ʍ\����
	char Text[MY_MAX_ALTERNATES][MAX_ELEMENT_NUM][256];//�F�����ʒP���
	char Word[MAX_ELEMENT_NUM][256];  // ��1���̒P���
	int taskflag; // 0�͖����o�A1�͕��u���A2�͕������A3�͖₢���킹(���u���������͔��f�ł��Ă��Ȃ�)

	int stateflag; //��ԃt���O:0�͏����A2�͎w�F������(�m�F���Ƃ�)�A3�͕��̔F������(�������s��)
	               //           1�͔F�����s(�r���ŏ����ɖ߂�)�A4�̓��[�U�ɏ���`����

	bool endflag;  //�����I��

	char time[3];
	char minute[3];

	int frame;  //���̉摜�̃t���[����

	int xmin[1],xmax[1],ymin[1],ymax[1];  //�w�����̈�(�����̈�)
	
	Cquery Database;   //�������s��
	myOutput dummy;      //�o�͗p�_�C�A���O

	//Multi_Disp SceneOut;  //��F�����̃V�[���o�͗p�_�C�A���O

	CQuerySlot Qslot,PreSlot;  //�₢���킹�X���b�g�I�u�W�F�N�g
	CTalkedContents user,system; //���b���e
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	////�ĉ��ߗp?+++++++++++++++++++++++++++++++++++++
	//int lnum,leftnum,mnum;
	//int LData[5],LtData[5],MData[5];
	////++++++++++++++++++++++++++++++++++++++++++++++

public:

	//�\���֌W+++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void ImageOut(unsigned char *pix);   //���A���^�C���摜�o��
	void Init(); //�F�X������
	void DispSystemmsg(const string &msg);//�u�V�X�e���̔��b�v�Ƀ��b�Z�[�W��\������֐�
	void DispUsermsg(const string &msg);//�u���[�U�̔��b�v�Ƀ��b�Z�[�W��\������֐�
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//�Θb����֘A+++++++++++++++++++++++++++++++++++++++++++++++++++
	void SetTask(){taskflag=GessTask();}
	           //0�F�����o�A1�F���u���A2�F������
	void ControlDialog(); //�Θb���䃋�[�`��
	int GessTask(); //�^�X�N�����肷��
	int IntoSlot(CQuerySlot& Q);  //�X���b�g�ɒl����
	int SplitResult();//�F�����ʂ�F���\���̂ɕ����ē����
	int SplitWord(char *string, char a, char b, 
			char *string1, 
			char *string2, 
			char *string3); //�P��ɂ����^�O(�����l)�����o��
	void recordInit(){ record = 0; } //�Θb�̗��������Z�b�g
	void taskflagInit(){taskflag=0;} //�^�X�N�t���O�����Z�b�g
	void stateflagInit(){stateflag=0;} //��ԃt���O�����Z�b�g
	void GenerateSystemMsg(CQuerySlot &Q); //�V�X�e���̔��b�𐶐�����
	void settask(int n){taskflag=n;}

	// �摜�����֘A++++++++++++++++++++++++++++++++++++++++++++++++++
	bool ReferenceObject(); //���[�U���w�����Ă���̈�𓾂�
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// �R���X�g���N�V����
public:
	CQueryByMouseDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_QUERYBYMOUSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOffline();
	afx_msg void OnBnClickedPut();
	afx_msg void OnBnClickedYes();
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedThanks();
	afx_msg void OnBnClickedLeave();
public:
	afx_msg void OnBnClickedOnline();
public:
	afx_msg void OnBnClickedTime();
public:
	afx_msg void OnBnClickedNum();
	int m_xvTime;
public:
	CComboBox m_xcTime;
};
