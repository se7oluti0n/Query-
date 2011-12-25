//-------------------------------------------------------------------
// FileName : FileOperator.cpp
// Context : �t�@�C������
// Description : FileOperator�N���X��`
// �쐬�� : �ЎR���� ( CV-Lab )
// �X�V�� : 2006.7.20
//-------------------------------------------------------------------
// [�Q�l]
// http://black.sakura.ne.jp/~third/system/winapi/common6.html

// How to Use
// SelectLoadFile("�J���t�@�C����I�����Ă�������","default.txt",".txt");

#include "stdafx.h"

#include <windows.h>
#include <string>
#include <ctime>
#include "FileOperator.h"


//---------------------------------------------------------
// Name  : FileOperator()
// Desc  : �R���X�g���N�^
// Input : �Ȃ�
// Output: �Ȃ�
//---------------------------------------------------------
FileOperator::FileOperator()
{
}


//-------------------------------------------------------------------------------------------------
// Name: SelectLoadFile()
// Desc: �ǂݍ��݃t�@�C����I�����t���p�X[fullpass]�A�I���t�@�C����[file]�A�p�X[pass]���l��
// Input:  �_�C�A���O�̃^�C�g���o�[�ɕ\�����镶����		char* Title
//		�@ �ǂݍ��݃t�@�C����							char* LoadFileName
//		   �g���q�w��									char* extension
// Output: �Ȃ�
//-------------------------------------------------------------------------------------------------
BOOL FileOperator::SelectLoadFile(const char* Title,const char* LoadFileName,const char* extension, std::string currentDir){

	char sFullPass[256]="";
	char Filename[256]="";
	char strCustomFilter[256]="";

	char cDir[256];
	GetCurrentDirectory(256,cDir);
	if(currentDir=="") currentDir = cDir;

	// �t�@�C���I��
	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof (OPENFILENAME);
	ofn.hwndOwner			= NULL; // �e�̃E�C���h�E�n���h��
	ofn.hInstance			= NULL; // �C���X�^���X�n���h��
	
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
	ofn.lpstrFile			= sFullPass;			// �t���p�X
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= Filename;				// �t�@�C����
	ofn.nMaxFileTitle		= MAX_PATH;
	ofn.lpstrInitialDir		= currentDir.c_str();	// �����t�@�C���f�B���N�g��
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
	
	// �p�X�����o��
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
// Desc: �������݃t�@�C����I������
// Input:  �_�C�A���O�̃^�C�g���o�[�ɕ\�����镶����		char* Title
//		�@ �������݃t�@�C����							char* SaveFileName
// Output: �Ȃ�
//--------------------------------------------------------------------------------------------
BOOL FileOperator::SelectSaveFile(const char* Title,const char* SaveFileName){

	char sFullPass[256]="";
	char Filename[256]="";
	char strCustomFilter[256]="";


	// �t�@�C���I��
	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof (OPENFILENAME);
	ofn.hwndOwner			= NULL; // �e�̃E�C���h�E�n���h��
	ofn.hInstance			= NULL; // �C���X�^���X�n���h��
	ofn.lpstrFilter = TEXT("Text files (*.txt)\0*.txt\0")
					TEXT("BITMAP files (*.bmp)\0*.bmp\0")
					TEXT("Jpeg files (*.jpg)\0*.jpg\0")
					TEXT("All files\0*.*\0\0");
	ofn.lpstrCustomFilter	= strCustomFilter;	//
	ofn.nMaxCustFilter		= 256;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= sFullPass; // �t���p�X
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= Filename;		// �t�@�C����
	ofn.nMaxFileTitle		= MAX_PATH;
	ofn.lpstrInitialDir		= NULL;			// �����t�@�C���f�B���N�g��
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

	// �p�X�����o��
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
// Desc: [pass]�Ƀf�B���N�g��[file - �g���q]�����
// Input:  �Ȃ�
// Output: �Ȃ�
//-----------------------------------------------------------------------------
void FileOperator::MakeDir(void){
	
	// �g���q�����
	std::string dir = file.substr(0,file.find_last_of('.'));
	// �f�B���N�g���쐬
	dirpass = pass;
	dirpass += dir;
	CreateDirectory(dirpass.c_str(),NULL);
	
	return;
}

//-----------------------------------------------------------------------------
// Name: MakeDir()
// Desc: [pass]�Ƀf�B���N�g��[DirName]�����
// Input:  �쐬����f�B���N�g����		char* DirName
// Output: �Ȃ�
//-----------------------------------------------------------------------------
void FileOperator::MakeDir(const char* DirName){

	// �f�B���N�g���쐬
	dirpass = pass;
	dirpass.insert(dirpass.size(),DirName);
	CreateDirectory(dirpass.c_str(),NULL);

	return;
}

//-----------------------------------------------------------------------------
// Name: MakeDir()
// Desc: �����̃t�@�C���ۑ��p�̃f�B���N�g�������
// Input:  �Ȃ�
// Output: �Ȃ�
//-----------------------------------------------------------------------------
void FileOperator::MakeFileDir()
{
	//���ݎ������f�B���N�g������
	time_t nt;
	struct tm *t_st;
	char c[30];
	//time(&nt);             //���ݎ����̎擾
	//t_st = localtime(&nt); //���ݎ������\���̂ɕϊ�
	localtime_s(t_st,&nt);
	strftime(c,sizeof(c),"%Y-%m-%d",t_st);

	//�t�@�C���ۑ��p��path
	pass =  "F:\\";

	// �f�B���N�g���쐬
	dirpass = pass;
	std::string DirName(c);
	dirpass.insert(dirpass.size(),DirName);
	CreateDirectory(dirpass.c_str(),NULL);

	return;
}