#pragma once

//-----------------
// Header File
//-----------------
#include <iostream>
#include <cstring>
#include "image.h"
using namespace std;

//-----------------
// Class
//-----------------
class bmp : public image {
private:
	/* �t�@�C���w�b�_ */
	unsigned short bfType;      // �t�@�C���^�C�v
	unsigned long  bfSize;      // �t�@�C���T�C�Y
	unsigned short bfReserved1; // �\��̈�
	unsigned short bfReserved2; // �\��̈�
	unsigned long  bfOffset;    // �I�t�Z�b�g

	/* ���w�b�_ */
	unsigned long  biSize;          // ���w�b�_�T�C�Y
	long  biWidth;                  // �摜�̕�
	long  biHeight;                 // �摜�̍���
	unsigned short biPlanes;        // �v���[����
	unsigned short biBitCount;      // 1��f�̃f�[�^�T�C�Y
	unsigned long  biCompression;   // ���k�`��
	unsigned long  biSizeImage;     // �摜�f�[�^�T�C�Y
	long  biXPelsPerMeter;          // �������̉𑜓x
	long  biYPelsPerMeter;          // �c�����̉𑜓x
	unsigned long  biClrUsed;       // �g�p�F��
	unsigned long  biClrImportant;  // �d�v�ȐF�̐�

public:
	
	// �R���X�g���N�^
	bmp() {};
	// �f�X�g���N�^
	~bmp() {};
	
	/* �摜�̃��[�h */
	bool load(const string filename);
	/* �摜�̃Z�[�u */
	bool save(const string filename);
	bool save2(const string filename);

	bool save_Y(const string filename);
	bool save_Y_d(const string filename);
	bool save_isDiff(const string filename);
};