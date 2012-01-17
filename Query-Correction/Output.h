#pragma once
#include "afxwin.h"
#include "bmp.h"
//#include "network.h"
// myOutput ダイアログ

class myOutput : public CDialog
{
	DECLARE_DYNAMIC(myOutput)

public:
	myOutput(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~myOutput();

// ダイアログ データ
	enum { IDD = IDD_OUTPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

public:
	void indicate(int frame,int task);
	bool ReadBitmap(CString file);

	void DispArea(int xmin,int xmax,int ymin,int ymax);
	void DispAreaAlt(bmp *input, int xmin,int xmax,int ymin,int ymax);

	void ShowResult(int frame);
	void ShowUndetectioScene(int folder,int frame);

	bmp I;
	BITMAPINFO bmpInfo; //bmpファイル用
	PAINTSTRUCT ps;
};
