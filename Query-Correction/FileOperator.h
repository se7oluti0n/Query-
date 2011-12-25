//-------------------------------------------------------------------
// FileName : FileOperator.h
// Context : �t�@�C������
// Description : FileOperator�N���X�錾
// �쐬�� : �ЎR���� ( CV-Lab )
// �X�V�� : 2006.7.20
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
	std::string file;		// �J���t�@�C����
	std::string fullpass;	// �t�@�C���̃t���p�X
	std::string pass;		// �t�@�C���܂ł̃p�X
	std::string dirpass;	// ������f�B���N�g���܂ł̃t���p�X

	FileOperator();
	BOOL SelectLoadFile(const char* Title,const char* LoadFileName,
		const char* extension, std::string currentDir="");	// �ǂݍ��݃t�@�C����I������
	BOOL SelectSaveFile(const char* ,const char* );	// �������݃t�@�C����I������
	void MakeDir(void);					// �f�B���N�g�������
	void MakeDir(const char* );			// �f�B���N�g�������
	void MakeFileDir(void);  //�t�@�C���ۑ��p�̃f�B���N�g�������
	std::string GetFilePass(){ return dirpass; }//�t�@�C����path��get
};