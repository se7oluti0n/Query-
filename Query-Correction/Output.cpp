// Output.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "QueryByMouse.h"
#include "Output.h"

// Output �_�C�A���O

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


// Output ���b�Z�[�W �n���h��

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
// ���ʉ摜���o��
// frame:�\�����錋�ʉ摜�̃t���[���ԍ�
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
// �m�F�摜�̕\��
// frame�F�\������BMP�t�@�C���̃t���[���ԍ�
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

	//�t�@�C�����擾
	if(task>=4 && task<=6){ //���u��
		//filename = "\\\\Proliant2/Maki/Detected-Object/";
		filename = "./Detected-Object/";
		temp1.Format("%d",frame);
		filename = filename + temp1 + "/result-" + temp1 + ".bmp";
	}else if(task>=7 && task<=9){ //������
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

	//�T���̈�͂�
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
// BITMAP�̓ǂݍ���
// frame�F�ǂݍ���BITMAP�̃t�@�C����
//
//-------------------------------------------
bool Output::ReadBitmap(CString filename)
{
	//CString ���� char �ɕϊ�
	char *name = new char[filename.GetLength()+1];
	strcpy(name,filename);

	bool f=I.load(name);

	delete [] name;

	return f;
}