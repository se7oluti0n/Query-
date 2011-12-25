// Output.cpp : 実装ファイル
//

#include "stdafx.h"
#include "QueryByMouse.h"
#include "Output.h"

// Output ダイアログ

IMPLEMENT_DYNAMIC(Output, CDialog)
Output::Output(CWnd* pParent /*=NULL*/)
	: CDialog(Output::IDD, pParent)
{
}

Output::~Output()
{
}

void Output::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Output, CDialog)
END_MESSAGE_MAP()


// Output メッセージ ハンドラ

void Output::ShowUndetectioScene(int folder, int frame)
{
	for(int i=-20;i<20;i++){
		CString file;
		CString temp1,temp2;

		file = "./Human-DB/Original/";
		temp1.Format("%lu",folder);
		temp2.Format("%08lu",frame+i);
		file = file + temp1 + "/" + temp2 + ".bmp";

		bool f=ReadBitmap(file);

		if(f==true){

			HWND hwndStill = NULL;
			GetDlgItem(IDC_RESULT,&hwndStill);

			HDC hdcStill = ::GetDC(hwndStill);
			::BeginPaint(hwndStill,&ps);
			SetStretchBltMode(hdcStill,COLORONCOLOR);
			
			StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
					&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

			::EndPaint(hwndStill,&ps);
			I.release();
		}
	}
}

//-----------------------------------------
// name:ShowResult(int frame)
// 結果画像を出力
// frame:表示する結果画像のフレーム番号
//
//-----------------------------------------
void Output::ShowResult(int frame)
{
	for(int i=9;i>0;i--){
		CString file;
		CString temp1,temp2;

		//file = "\\\\Proliant2/Maki/Detected-Object/";
		file = "./Detected-Object/";
		temp1.Format("%d",frame);
		temp2.Format("%02lu",i);
		file = file + temp1 + "/t-" + temp2 + "-" + temp1 + ".bmp";

		ReadBitmap(file);

		HWND hwndStill = NULL;
		GetDlgItem(IDC_RESULT,&hwndStill);

		HDC hdcStill = ::GetDC(hwndStill);
		::BeginPaint(hwndStill,&ps);
		SetStretchBltMode(hdcStill,COLORONCOLOR);
		
		StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
				&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

		::EndPaint(hwndStill,&ps);
		Sleep(100);
		I.release();
	}

}

//-------------------------------------------
// name:indicate(int frame)
// 確認画像の表示
// frame：表示するBMPファイルのフレーム番号
//
//-------------------------------------------
void Output::indicate(int frame,int task)
{
	CString filename;
	CString temp1;
	CString temp2;

	bmpInfo.bmiHeader.biHeight        = 240;
	bmpInfo.bmiHeader.biWidth         = 320;
	bmpInfo.bmiHeader.biBitCount      = 24;
	bmpInfo.bmiHeader.biClrImportant  = 0;
	bmpInfo.bmiHeader.biClrUsed       = 0;
	bmpInfo.bmiHeader.biCompression   = 0;
	bmpInfo.bmiHeader.biPlanes        = 1;
	bmpInfo.bmiHeader.biSize          = 40;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;

	//ファイル名取得
	if(task>=4 && task<=6){ //放置物
		//filename = "\\\\Proliant2/Maki/Detected-Object/";
		filename = "./Detected-Object/";
		temp1.Format("%d",frame);
		filename = filename + temp1 + "/result-" + temp1 + ".bmp";
	}else if(task>=7 && task<=9){ //紛失物
		//filename = "\\\\Proliant2/Maki/Detected-Object/";
		filename = "./Detected-Object/";
		temp1.Format("%d",frame);
		temp2.Format("%02lu",6);
		filename = filename + temp1 + "/t-" + temp2 + "-" + temp1 + ".bmp";
	}

	ReadBitmap(filename);

	HWND hwndStill = NULL;
	GetDlgItem(IDC_RESULT,&hwndStill);

	HDC hdcStill = ::GetDC(hwndStill);
	::BeginPaint(hwndStill,&ps);
	SetStretchBltMode(hdcStill,COLORONCOLOR);
	
	StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
			&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

	::EndPaint(hwndStill,&ps);

	I.release();
}

void Output::DispArea(int xmin,int xmax,int ymin,int ymax)
{
	bmpInfo.bmiHeader.biHeight        = 240;
	bmpInfo.bmiHeader.biWidth         = 320;
	bmpInfo.bmiHeader.biBitCount      = 24;
	bmpInfo.bmiHeader.biClrImportant  = 0;
	bmpInfo.bmiHeader.biClrUsed       = 0;
	bmpInfo.bmiHeader.biCompression   = 0;
	bmpInfo.bmiHeader.biPlanes        = 1;
	bmpInfo.bmiHeader.biSize          = 40;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;

	ReadBitmap("0.bmp");

	HWND hwndStill = NULL;
	GetDlgItem(IDC_RESULT,&hwndStill);

	HDC hdcStill = ::GetDC(hwndStill);
	::BeginPaint(hwndStill,&ps);
	SetStretchBltMode(hdcStill,COLORONCOLOR);

	//探索領域囲む
	for(int j=ymin;j<ymax;j++){
		I.B(xmin,j)=255; I.G(xmin,j)=0; I.R(xmin,j)=0;
		I.B(xmax,j)=255; I.G(xmax,j)=0; I.R(xmax,j)=0;
	}
	for(int j=xmin;j<xmax;j++){
		I.B(j,ymin)=255; I.G(j,ymin)=0; I.R(j,ymin)=0;
		I.B(j,ymax)=255; I.G(j,ymax)=0; I.R(j,ymax)=0;
	}
	
	StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
			&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

	::EndPaint(hwndStill,&ps);

	I.release();
}

//-------------------------------------------
// name:ReadBitmap(CString filename)
// BITMAPの読み込み
// frame：読み込むBITMAPのファイル名
//
//-------------------------------------------
bool Output::ReadBitmap(CString filename)
{
	//CString から char に変換
	char *name = new char[filename.GetLength()+1];
	strcpy(name,filename);

	bool f=I.load(name);

	delete [] name;

	return f;
}