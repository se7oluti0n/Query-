//-----------------
// Header File
//-----------------
#include "stdafx.h"
#include <fstream>
#include <string>
#include <cmath>
#include "bmp.h"

long line;  // ���C���̃f�[�^��

/****************/
/* �摜�̃��[�h */
/****************/
bool bmp::load(const string filename)
{
	ifstream fin;
	long i,j,position;
	
	/* �t�@�C���I�[�v�� */
	fin.open(filename.c_str(),ios::binary);
	if(!fin){
		cout << "Cannot open file : " << filename << '\n';
		return false;
	}
	
	/* �w�b�_���擾 */
	/* �t�@�C���w�b�_���̎擾 */
	fin.read((char *)&bfType,2);      // �t�@�C���^�C�v
	fin.read((char *)&bfSize,4);      // �t�@�C���T�C�Y
	fin.read((char *)&bfReserved1,2); // �\��̈�
	fin.read((char *)&bfReserved2,2); // �\��̈�
	fin.read((char *)&bfOffset,4);    // �I�t�Z�b�g

	/* ���w�b�_�̎擾 */
	fin.read((char *)&biSize,4);          // ���w�b�h�T�C�Y
	fin.read((char *)&biWidth,4);         // �摜�̕�
	fin.read((char *)&biHeight,4);        // �摜�̍���
	fin.read((char *)&biPlanes,2);        // �v���[����
	fin.read((char *)&biBitCount,2);      // 1��f�̃f�[�^�T�C�Y
	fin.read((char *)&biCompression,4);   // ���k�`��	
	fin.read((char *)&biSizeImage,4);     // �摜�f�[�^�T�C�Y	
	fin.read((char *)&biXPelsPerMeter,4); // �������̉𑜓x	
	fin.read((char *)&biYPelsPerMeter,4); // �c�����̉𑜓x	
	fin.read((char *)&biClrUsed,4);       // �g�p�F��	
	fin.read((char *)&biClrImportant,4);  // �d�v�ȐF�̐�
	
	/* �摜�f�[�^�̓ǂݍ��� */
	/* �摜�f�[�^�̗̈���m�� */
	init();
	
	/* 1���C���̃f�[�^����4byte���E�ɍ��킹�� */
	line = (biWidth*biBitCount)/8;
	if((line%4)!=0){
		line = ((line/4)+1)*4;
	}
	
	for(i=biHeight-1;i>0;i--){
		/* �s�̐擪�ʒu */
		position=bfOffset+line*i;
		fin.seekg(position,ios::beg);
		
		/* �l�̓ǂݍ��� */
		for(j=0;j<biWidth;j++){
			fin.read((char *)&B(j,i),1);
			fin.read((char *)&G(j,i),1);
			fin.read((char *)&R(j,i),1);
		}
	}
	
	fin.close();  // �t�@�C���N���[�Y
	
	return true;
}

/****************/
/* �摜�̃Z�[�u */
/****************/
bool bmp::save(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* �t�@�C���I�[�v�� */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}
	
	/* �t�@�C���w�b�_�̏������� */
	fout.write((char *)&bfType,2);       // �t�@�C���^�C�v
	fout.write((char *)&bfSize,4);       // �t�@�C���T�C�Y
	fout.write((char *)&bfReserved1,2);  // �\��̈�
	fout.write((char *)&bfReserved2,2);  // �\��̈�
	fout.write((char *)&bfOffset,4);     // �I�t�Z�b�g
	
	/* ���w�b�_�̏������� */
	fout.write((char *)&biSize,4);          // ���w�b�_�T�C�Y
	fout.write((char *)&biWidth,4);         // �摜�̕�
	fout.write((char *)&biHeight,4);        // �摜�̍���
	fout.write((char *)&biPlanes,2);        // �v���[����
	fout.write((char *)&biBitCount,2);      // 1��f�̃f�[�^�T�C�Y
	fout.write((char *)&biCompression,4);   // ���k�`��
	fout.write((char *)&biSizeImage,4);     // �摜�f�[�^
	fout.write((char *)&biXPelsPerMeter,4); // �������̉𑜓x
	fout.write((char *)&biYPelsPerMeter,4); // �c�����̉𑜓x
	fout.write((char *)&biClrUsed,4);       // �g�p�F��
	fout.write((char *)&biClrImportant,4);  // �d�v�ȐF�̐�
	
	/************************/
	/* �摜�f�[�^�̏������� */
	/************************/
	/* �s�̐擪�ʒu */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* �l�̏������� */
		for(j=0;j<biWidth;j++){
			fout.write((char *)&B(j,i),1);
			fout.write((char *)&G(j,i),1);
			fout.write((char *)&R(j,i),1);
		}
		/* 1���C���̃f�[�^����4byte���E�ɍ��킹�� */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // �t�@�C���N���[�Y
	
	return true;
}

/****************/
/* �摜�̃Z�[�u */
/****************/
bool bmp::save2(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* �t�@�C���I�[�v�� */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}
	
	/* �t�@�C���w�b�_�̏������� */
	fout.write((char *)&bfType,2);       // �t�@�C���^�C�v
	fout.write((char *)&bfSize,4);       // �t�@�C���T�C�Y
	fout.write((char *)&bfReserved1,2);  // �\��̈�
	fout.write((char *)&bfReserved2,2);  // �\��̈�
	fout.write((char *)&bfOffset,4);     // �I�t�Z�b�g
	
	/* ���w�b�_�̏������� */
	fout.write((char *)&biSize,4);          // ���w�b�_�T�C�Y
	fout.write((char *)&biWidth,4);         // �摜�̕�
	fout.write((char *)&biHeight,4);        // �摜�̍���
	fout.write((char *)&biPlanes,2);        // �v���[����
	fout.write((char *)&biBitCount,2);      // 1��f�̃f�[�^�T�C�Y
	fout.write((char *)&biCompression,4);   // ���k�`��
	fout.write((char *)&biSizeImage,4);     // �摜�f�[�^
	fout.write((char *)&biXPelsPerMeter,4); // �������̉𑜓x
	fout.write((char *)&biYPelsPerMeter,4); // �c�����̉𑜓x
	fout.write((char *)&biClrUsed,4);       // �g�p�F��
	fout.write((char *)&biClrImportant,4);  // �d�v�ȐF�̐�
	
	/************************/
	/* �摜�f�[�^�̏������� */
	/************************/
	/* �s�̐擪�ʒu */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* �l�̏������� */
		for(j=0;j<biWidth;j++){
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
		}
		/* 1���C���̃f�[�^����4byte���E�ɍ��킹�� */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // �t�@�C���N���[�Y
	
	return true;
}

/****************/
/* �摜�̃Z�[�u */
/****************/
bool bmp::save_Y(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* �t�@�C���I�[�v�� */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}

	//init file header
	bfType    = 0x4D42;
	bfSize	  = 14+40+line*240;
	bfOffset = 54;

	//init info header
	biSize = 40;
	biWidth = 320;
	biHeight = 240;
	biPlanes = 1;
	biBitCount = 24;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	
	/* �t�@�C���w�b�_�̏������� */
	fout.write((char *)&bfType,2);       // �t�@�C���^�C�v
	fout.write((char *)&bfSize,4);       // �t�@�C���T�C�Y
	fout.write((char *)&bfReserved1,2);  // �\��̈�
	fout.write((char *)&bfReserved2,2);  // �\��̈�
	fout.write((char *)&bfOffset,4);     // �I�t�Z�b�g
	
	/* ���w�b�_�̏������� */
	fout.write((char *)&biSize,4);          // ���w�b�_�T�C�Y
	fout.write((char *)&biWidth,4);         // �摜�̕�
	fout.write((char *)&biHeight,4);        // �摜�̍���
	fout.write((char *)&biPlanes,2);        // �v���[����
	fout.write((char *)&biBitCount,2);      // 1��f�̃f�[�^�T�C�Y
	fout.write((char *)&biCompression,4);   // ���k�`��
	fout.write((char *)&biSizeImage,4);     // �摜�f�[�^
	fout.write((char *)&biXPelsPerMeter,4); // �������̉𑜓x
	fout.write((char *)&biYPelsPerMeter,4); // �c�����̉𑜓x
	fout.write((char *)&biClrUsed,4);       // �g�p�F��
	fout.write((char *)&biClrImportant,4);  // �d�v�ȐF�̐�
	
	/************************/
	/* �摜�f�[�^�̏������� */
	/************************/
	/* �s�̐擪�ʒu */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* �l�̏������� */
		for(j=0;j<biWidth;j++){
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
		}
		/* 1���C���̃f�[�^����4byte���E�ɍ��킹�� */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // �t�@�C���N���[�Y
	
	return true;
}

/****************/
/* �摜�̃Z�[�u */
/****************/
bool bmp::save_Y_d(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* �t�@�C���I�[�v�� */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}

	//init file header
	bfType    = 0x4D42;
	bfSize	  = 14+40+line*240;
	bfOffset = 54;

	//init info header
	biSize = 40;
	biWidth = 320;
	biHeight = 240;
	biPlanes = 1;
	biBitCount = 24;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	
	/* �t�@�C���w�b�_�̏������� */
	fout.write((char *)&bfType,2);       // �t�@�C���^�C�v
	fout.write((char *)&bfSize,4);       // �t�@�C���T�C�Y
	fout.write((char *)&bfReserved1,2);  // �\��̈�
	fout.write((char *)&bfReserved2,2);  // �\��̈�
	fout.write((char *)&bfOffset,4);     // �I�t�Z�b�g
	
	/* ���w�b�_�̏������� */
	fout.write((char *)&biSize,4);          // ���w�b�_�T�C�Y
	fout.write((char *)&biWidth,4);         // �摜�̕�
	fout.write((char *)&biHeight,4);        // �摜�̍���
	fout.write((char *)&biPlanes,2);        // �v���[����
	fout.write((char *)&biBitCount,2);      // 1��f�̃f�[�^�T�C�Y
	fout.write((char *)&biCompression,4);   // ���k�`��
	fout.write((char *)&biSizeImage,4);     // �摜�f�[�^
	fout.write((char *)&biXPelsPerMeter,4); // �������̉𑜓x
	fout.write((char *)&biYPelsPerMeter,4); // �c�����̉𑜓x
	fout.write((char *)&biClrUsed,4);       // �g�p�F��
	fout.write((char *)&biClrImportant,4);  // �d�v�ȐF�̐�
	
	/************************/
	/* �摜�f�[�^�̏������� */
	/************************/
	/* �s�̐擪�ʒu */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* �l�̏������� */
		for(j=0;j<biWidth;j++){
			Y(j,i)=(unsigned char)Y_d(j,i);
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
		}
		/* 1���C���̃f�[�^����4byte���E�ɍ��킹�� */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // �t�@�C���N���[�Y
	
	return true;
}

/****************/
/* �摜�̃Z�[�u */
/****************/
bool bmp::save_isDiff(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* �t�@�C���I�[�v�� */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}

	//init file header
	bfType    = 0x4D42;
	bfSize	  = 14+40+line*240;
	bfOffset = 54;

	//init info header
	biSize = 40;
	biWidth = 320;
	biHeight = 240;
	biPlanes = 1;
	biBitCount = 24;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	
	/* �t�@�C���w�b�_�̏������� */
	fout.write((char *)&bfType,2);       // �t�@�C���^�C�v
	fout.write((char *)&bfSize,4);       // �t�@�C���T�C�Y
	fout.write((char *)&bfReserved1,2);  // �\��̈�
	fout.write((char *)&bfReserved2,2);  // �\��̈�
	fout.write((char *)&bfOffset,4);     // �I�t�Z�b�g
	
	/* ���w�b�_�̏������� */
	fout.write((char *)&biSize,4);          // ���w�b�_�T�C�Y
	fout.write((char *)&biWidth,4);         // �摜�̕�
	fout.write((char *)&biHeight,4);        // �摜�̍���
	fout.write((char *)&biPlanes,2);        // �v���[����
	fout.write((char *)&biBitCount,2);      // 1��f�̃f�[�^�T�C�Y
	fout.write((char *)&biCompression,4);   // ���k�`��
	fout.write((char *)&biSizeImage,4);     // �摜�f�[�^
	fout.write((char *)&biXPelsPerMeter,4); // �������̉𑜓x
	fout.write((char *)&biYPelsPerMeter,4); // �c�����̉𑜓x
	fout.write((char *)&biClrUsed,4);       // �g�p�F��
	fout.write((char *)&biClrImportant,4);  // �d�v�ȐF�̐�
	
	/************************/
	/* �摜�f�[�^�̏������� */
	/************************/
	/* �s�̐擪�ʒu */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* �l�̏������� */
		for(j=0;j<biWidth;j++){
			if(isDiff(j,i)==0)  isDiff(j,i)=0;
			else	isDiff(j,i)=255;
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
		}
		/* 1���C���̃f�[�^����4byte���E�ɍ��킹�� */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // �t�@�C���N���[�Y
	
	return true;
}