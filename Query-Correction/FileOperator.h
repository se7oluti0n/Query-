//-------------------------------------------------------------------
// FileName : FileOperator.h
// Context : ファイル操作
// Description : FileOperatorクラス宣言
// 作成者 : 片山憲昭 ( CV-Lab )
// 更新日 : 2006.7.20
//-------------------------------------------------------------------
#pragma once
#include <windows.h>
#include <string>


//---------------------------------------------------------
// class
//---------------------------------------------------------
class FileOperator
{

public:
	std::string file;		// 開くファイル名
	std::string fullpass;	// ファイルのフルパス
	std::string pass;		// ファイルまでのパス
	std::string dirpass;	// 作ったディレクトリまでのフルパス

	FileOperator();
	BOOL SelectLoadFile(const char* Title,const char* LoadFileName,
		const char* extension, std::string currentDir="");	// 読み込みファイルを選択する
	BOOL SelectSaveFile(const char* ,const char* );	// 書き込みファイルを選択する
	void MakeDir(void);					// ディレクトリを作る
	void MakeDir(const char* );			// ディレクトリを作る
	void MakeFileDir(void);  //ファイル保存用のディレクトリを作る
	std::string GetFilePass(){ return dirpass; }//ファイルのpathをget
};