#pragma once
#include "bmp.h"
#include "query.h"

// Multi_Disp �_�C�A���O

class Multi_Disp : public CDialog
{
	DECLARE_DYNAMIC(Multi_Disp)

public:
	Multi_Disp(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~Multi_Disp();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_MULTI_DISP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()

public:
	bmp I;
	BITMAPINFO bmpInfo; //bmp�t�@�C���p
	PAINTSTRUCT ps;
	int StopFlag;

	void indicate(Object *DispData,int DispNum);
	void indicate_Undetection(int *DispFolder, int *DispFrame, int DispNum);
	bool ReadBitmap(CString file);

	void Stop(){StopFlag=1;}
};