#pragma once
#include "afxwin.h"
#include "bmp.h"


// Output �_�C�A���O

class Output : public CDialog
{
	DECLARE_DYNAMIC(Output)

public:
	Output(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~Output();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OUTPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()

public:
	void indicate(int frame,int task);
	bool ReadBitmap(CString file);

	void DispArea(int xmin,int xmax,int ymin,int ymax);

	void ShowResult(int frame);
	void ShowUndetectioScene(int folder,int frame);

	bmp I;
	BITMAPINFO bmpInfo; //bmp�t�@�C���p
	PAINTSTRUCT ps;
};
