#pragma once
#include "bmp.h"
#include "query.h"

// Multi_Disp ダイアログ

class Multi_Disp : public CDialog
{
	DECLARE_DYNAMIC(Multi_Disp)

public:
	Multi_Disp(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~Multi_Disp();

// ダイアログ データ
	enum { IDD = IDD_MULTI_DISP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

public:
	bmp I;
	BITMAPINFO bmpInfo; //bmpファイル用
	PAINTSTRUCT ps;
	int StopFlag;

	void indicate(Object *DispData,int DispNum);
	void indicate_Undetection(int *DispFolder, int *DispFrame, int DispNum);
	bool ReadBitmap(CString file);

	void Stop(){StopFlag=1;}
};