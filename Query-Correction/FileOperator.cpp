//-------------------------------------------------------------------
// FileName : FileOperator.cpp
// Context : ファイル操作
// Description : FileOperatorクラス定義
// 作成者 : 片山憲昭 ( CV-Lab )
// 更新日 : 2006.7.20
//-------------------------------------------------------------------
// [参考]
// http://black.sakura.ne.jp/~third/system/winapi/common6.html

// How to Use
// SelectLoadFile("開くファイルを選択してください","default.txt",".txt");

#include "stdafx.h"

#include <windows.h>
#include <string>
#include <ctime>
#include "FileOperator.h"


//---------------------------------------------------------
// Name  : FileOperator()
// Desc  : コンストラクタ
// Input : なし
// Output: なし
//---------------------------------------------------------
FileOperator::FileOperator()
{
}


//-------------------------------------------------------------------------------------------------
// Name: SelectLoadFile()
// Desc: 読み込みファイルを選択しフルパス[fullpass]、選択ファイル名[file]、パス[pass]を獲得
// Input:  ダイアログのタイトルバーに表示する文字列		char* Title
//		　 読み込みファイル名							char* LoadFileName
//		   拡張子指定									char* extension
// Output: なし
//-------------------------------------------------------------------------------------------------
BOOL FileOperator::SelectLoadFile(const char* Title,const char* LoadFileName,const char* extension, std::string currentDir){

	char sFullPass[256]="";
	char Filename[256]="";
	char strCustomFilter[256]="";

	char cDir[256];
	GetCurrentDirectory(256,cDir);
	if(currentDir=="") currentDir = cDir;

	// ファイル選択
	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof (OPENFILENAME);
	ofn.hwndOwner			= NULL; // 親のウインドウハンドル
	ofn.hInstance			= NULL; // インスタンスハンドル
	
	if(strcmp(extension,".txt") == 0)
		ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0\0";
	else if(strcmp(extension,".bmp") == 0)
		ofn.lpstrFilter = "BITMAP files (*.bmp)\0*.bmp\0\0";
	else if(strcmp(extension,".jpg") == 0)
		ofn.lpstrFilter = "Jpeg files (*.jpg)\0*.jpg\0\0";
	else
		ofn.lpstrFilter = "All files\0*.*\0\0";

	ofn.lpstrCustomFilter	= strCustomFilter;	//
	ofn.nMaxCustFilter		= 256;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= sFullPass;			// フルパス
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= Filename;				// ファイル名
	ofn.nMaxFileTitle		= MAX_PATH;
	ofn.lpstrInitialDir		= currentDir.c_str();	// 初期ファイルディレクトリ
	ofn.lpstrTitle			= Title;
	ofn.Flags				= OFN_FILEMUSTEXIST;
	ofn.nFileOffset			= NULL;
	ofn.nFileExtension		= NULL;
	ofn.lpstrDefExt			= NULL;
	ofn.lCustData			= NULL;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;

	//
	strcpy_s(sFullPass,LoadFileName);
	BOOL flag = GetOpenFileName(&ofn);
	
	// パス抜き出し
	fullpass.clear();
	file.clear();
	pass.clear();
	// Full File Pass
	fullpass = sFullPass;
	// Filename
	file = Filename;
	// Current Directry
	pass = fullpass.substr(0,fullpass.find_last_of('\\')+1);
	
	return flag;
}


//--------------------------------------------------------------------------------------------
// Name: SelectSaveFile()
// Desc: 書き込みファイルを選択する
// Input:  ダイアログのタイトルバーに表示する文字列		char* Title
//		　 書き込みファイル名							char* SaveFileName
// Output: なし
//--------------------------------------------------------------------------------------------
BOOL FileOperator::SelectSaveFile(const char* Title,const char* SaveFileName){

	char sFullPass[256]="";
	char Filename[256]="";
	char strCustomFilter[256]="";


	// ファイル選択
	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof (OPENFILENAME);
	ofn.hwndOwner			= NULL; // 親のウインドウハンドル
	ofn.hInstance			= NULL; // インスタンスハンドル
	ofn.lpstrFilter = TEXT("Text files (*.txt)\0*.txt\0")
					TEXT("BITMAP files (*.bmp)\0*.bmp\0")
					TEXT("Jpeg files (*.jpg)\0*.jpg\0")
					TEXT("All files\0*.*\0\0");
	ofn.lpstrCustomFilter	= strCustomFilter;	//
	ofn.nMaxCustFilter		= 256;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= sFullPass; // フルパス
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= Filename;		// ファイル名
	ofn.nMaxFileTitle		= MAX_PATH;
	ofn.lpstrInitialDir		= NULL;			// 初期ファイルディレクトリ
	ofn.lpstrTitle			= Title;
	ofn.Flags				= OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.nFileOffset			= NULL;
	ofn.nFileExtension		= NULL;
	ofn.lpstrDefExt			= NULL;
	ofn.lCustData			= NULL;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
	
	//
	strcpy_s(sFullPass,SaveFileName);
	BOOL flag = GetSaveFileName(&ofn);

	// パス抜き出し
	fullpass.clear();
	file.clear();
	pass.clear();
	fullpass = sFullPass;
	file = Filename;
	pass = fullpass.substr(0,fullpass.find_last_of('\\')+1);

	return flag;
}

//-----------------------------------------------------------------------------
// Name: MakeDir()
// Desc: [pass]にディレクトリ[file - 拡張子]を作る
// Input:  なし
// Output: なし
//-----------------------------------------------------------------------------
void FileOperator::MakeDir(void){
	
	// 拡張子を取る
	std::string dir = file.substr(0,file.find_last_of('.'));
	// ディレクトリ作成
	dirpass = pass;
	dirpass += dir;
	CreateDirectory(dirpass.c_str(),NULL);
	
	return;
}

//-----------------------------------------------------------------------------
// Name: MakeDir()
// Desc: [pass]にディレクトリ[DirName]を作る
// Input:  作成するディレクトリ名		char* DirName
// Output: なし
//-----------------------------------------------------------------------------
void FileOperator::MakeDir(const char* DirName){

	// ディレクトリ作成
	dirpass = pass;
	dirpass.insert(dirpass.size(),DirName);
	CreateDirectory(dirpass.c_str(),NULL);

	return;
}

//-----------------------------------------------------------------------------
// Name: MakeDir()
// Desc: 実験のファイル保存用のディレクトリを作る
// Input:  なし
// Output: なし
//-----------------------------------------------------------------------------
void FileOperator::MakeFileDir()
{
	//現在時刻をディレクトリ名に
	time_t nt;
	struct tm *t_st;
	char c[30];
	//time(&nt);             //現在時刻の取得
	//t_st = localtime(&nt); //現在時刻を構造体に変換
	localtime_s(t_st,&nt);
	strftime(c,sizeof(c),"%Y-%m-%d",t_st);

	//ファイル保存用のpath
	pass =  "F:\\";

	// ディレクトリ作成
	dirpass = pass;
	std::string DirName(c);
	dirpass.insert(dirpass.size(),DirName);
	CreateDirectory(dirpass.c_str(),NULL);

	return;
}