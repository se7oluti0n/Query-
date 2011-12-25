// Multi_Disp.cpp : 実装ファイル
//

#include "stdafx.h"
#include "QueryByMouse.h"
#include "Multi_Disp.h"


// Multi_Disp ダイアログ

IMPLEMENT_DYNAMIC(Multi_Disp, CDialog)

Multi_Disp::Multi_Disp(CWnd* pParent /*=NULL*/)
	: CDialog(Multi_Disp::IDD, pParent)
{

}

Multi_Disp::~Multi_Disp()
{
}

void Multi_Disp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Multi_Disp, CDialog)
END_MESSAGE_MAP()


// Multi_Disp メッセージ ハンドラ

void Multi_Disp::indicate(Object *DispData,int DispNum)
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

	StopFlag=0;

	this->SetWindowTextA("Scene in Event-DB");

	for(int i=9;i>=0;i--){
		if(StopFlag==1)
			break;

		//一つ目出力
		CString file;
		CString temp1,temp2;

		if(i==0){
			file = "./Detected-Object/";
			temp1.Format("%d",DispData[0].frame);
			file = file + temp1 + "/result-" + temp1 + ".bmp";
		}else{
			//file = "\\\\Proliant2/Maki/Detected-Object/";
			file = "./Detected-Object/";
			temp1.Format("%d",DispData[0].frame);
			temp2.Format("%02lu",i);
			file = file + temp1 + "/t-" + temp2 + "-" + temp1 + ".bmp";
		}

		ReadBitmap(file);

		HWND hwndStill = NULL;
		GetDlgItem(IDC_IMAGE1,&hwndStill);

		HDC hdcStill = ::GetDC(hwndStill);
		::BeginPaint(hwndStill,&ps);
		SetStretchBltMode(hdcStill,COLORONCOLOR);
		
		StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
				&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

		::EndPaint(hwndStill,&ps);
		I.release();

		//二つ目出力
		if(DispNum>=2){
			if(i==0){
				file = "./Detected-Object/";
				temp1.Format("%d",DispData[1].frame);
				file = file + temp1 + "/result-" + temp1 + ".bmp";
			}else{
				//file = "\\\\Proliant2/Maki/Detected-Object/";
				file = "./Detected-Object/";
				temp1.Format("%d",DispData[1].frame);
				temp2.Format("%02lu",i);
				file = file + temp1 + "/t-" + temp2 + "-" + temp1 + ".bmp";
			}

			ReadBitmap(file);

			HWND hwndStill = NULL;
			GetDlgItem(IDC_IMAGE2,&hwndStill);

			HDC hdcStill = ::GetDC(hwndStill);
			::BeginPaint(hwndStill,&ps);
			SetStretchBltMode(hdcStill,COLORONCOLOR);
			
			StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
					&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

			::EndPaint(hwndStill,&ps);
			I.release();
		}

		//三つ目出力
		if(DispNum>=3){
			if(i==0){
				file = "./Detected-Object/";
				temp1.Format("%d",DispData[2].frame);
				file = file + temp1 + "/result-" + temp1 + ".bmp";
			}else{
				//file = "\\\\Proliant2/Maki/Detected-Object/";
				file = "./Detected-Object/";
				temp1.Format("%d",DispData[2].frame);
				temp2.Format("%02lu",i);
				file = file + temp1 + "/t-" + temp2 + "-" + temp1 + ".bmp";
			}

			ReadBitmap(file);

			HWND hwndStill = NULL;
			GetDlgItem(IDC_IMAGE3,&hwndStill);

			HDC hdcStill = ::GetDC(hwndStill);
			::BeginPaint(hwndStill,&ps);
			SetStretchBltMode(hdcStill,COLORONCOLOR);
			
			StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
					&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

			::EndPaint(hwndStill,&ps);
			I.release();
		}

		//四つ目出力
		if(DispNum>=4){
			if(i==0){
				file = "./Detected-Object/";
				temp1.Format("%d",DispData[3].frame);
				file = file + temp1 + "/result-" + temp1 + ".bmp";
			}else{
				//file = "\\\\Proliant2/Maki/Detected-Object/";
				file = "./Detected-Object/";
				temp1.Format("%d",DispData[3].frame);
				temp2.Format("%02lu",i);
				file = file + temp1 + "/t-" + temp2 + "-" + temp1 + ".bmp";
			}

			ReadBitmap(file);

			HWND hwndStill = NULL;
			GetDlgItem(IDC_IMAGE4,&hwndStill);

			HDC hdcStill = ::GetDC(hwndStill);
			::BeginPaint(hwndStill,&ps);
			SetStretchBltMode(hdcStill,COLORONCOLOR);
			
			StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
					&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

			::EndPaint(hwndStill,&ps);
			I.release();
		}

		if(i==0)	i=10;
	}
}

void Multi_Disp::indicate_Undetection(int *DispFolder, int *DispFrame, int DispNum)
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

	StopFlag=0;

	this->SetWindowTextA("Scene in Human-DB");

	for(int i=-20;i<20;i++){
		if(StopFlag==1)
			break;
		//一つ目出力
		CString file;
		CString temp1,temp2;

		file = "./Human-DB/Original/";
		temp1.Format("%lu",DispFolder[0]);
		temp2.Format("%08lu",DispFrame[0]+i);
		file = file + temp1 + "/" + temp2 + ".bmp";

		bool f=ReadBitmap(file);

		if(f==true){

			HWND hwndStill = NULL;
			GetDlgItem(IDC_IMAGE1,&hwndStill);

			HDC hdcStill = ::GetDC(hwndStill);
			::BeginPaint(hwndStill,&ps);
			SetStretchBltMode(hdcStill,COLORONCOLOR);
			
			StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
					&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

			::EndPaint(hwndStill,&ps);
			I.release();
		}

		//二つ目出力
		if(DispNum>=2){
			file = "./Human-DB/Original/";
			temp1.Format("%lu",DispFolder[1]);
			temp2.Format("%08lu",DispFrame[1]+i);
			file = file + temp1 + "/" + temp2 + ".bmp";

			bool f=ReadBitmap(file);

			if(f==true){
				HWND hwndStill = NULL;
				GetDlgItem(IDC_IMAGE2,&hwndStill);

				HDC hdcStill = ::GetDC(hwndStill);
				::BeginPaint(hwndStill,&ps);
				SetStretchBltMode(hdcStill,COLORONCOLOR);
				
				StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
						&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

				::EndPaint(hwndStill,&ps);
				I.release();
			}
		}

		//三つ目出力
		if(DispNum>=3){
			file = "./Human-DB/Original/";
			temp1.Format("%lu",DispFolder[2]);
			temp2.Format("%08lu",DispFrame[2]+i);
			file = file + temp1 + "/" + temp2 + ".bmp";

			bool f=ReadBitmap(file);

			if(f==true){
				HWND hwndStill = NULL;
				GetDlgItem(IDC_IMAGE3,&hwndStill);

				HDC hdcStill = ::GetDC(hwndStill);
				::BeginPaint(hwndStill,&ps);
				SetStretchBltMode(hdcStill,COLORONCOLOR);
				
				StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
						&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

				::EndPaint(hwndStill,&ps);
				I.release();
			}
		}

		//四つ目出力
		if(DispNum>=4){
			file = "./Human-DB/Original/";
			temp1.Format("%lu",DispFolder[3]);
			temp2.Format("%08lu",DispFrame[3]+i);
			file = file + temp1 + "/" + temp2 + ".bmp";

			bool f=ReadBitmap(file);

			if(f==true){
				HWND hwndStill = NULL;
				GetDlgItem(IDC_IMAGE4,&hwndStill);

				HDC hdcStill = ::GetDC(hwndStill);
				::BeginPaint(hwndStill,&ps);
				SetStretchBltMode(hdcStill,COLORONCOLOR);
				
				StretchDIBits(hdcStill,0,0,320,240,0,0,320,240,
						&I.B(0,0),&bmpInfo,DIB_RGB_COLORS,SRCCOPY);

				::EndPaint(hwndStill,&ps);
				I.release();
			}
		}

		if(i==19)	i=-20;
	}
}

//-------------------------------------------
// name:ReadBitmap(CString filename)
// BITMAPの読み込み
// frame：読み込むBITMAPのファイル名
//
//-------------------------------------------
bool Multi_Disp::ReadBitmap(CString filename)
{
	//CString から char に変換
	char *name = new char[filename.GetLength()+1];
	strcpy(name,filename);

	bool f=I.load(name);

	delete [] name;
	
	return f;
}
