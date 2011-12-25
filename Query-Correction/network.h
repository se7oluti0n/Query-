#pragma once

#include "bmp.h"
#include "QueryByMouse.h"
#include "QueryByMouseDlg.h"
#include <WinSock2.h>
#include "cv.h"
class CQueryByMouseDlg;

//-----------------
// Class
//-----------------
class CCamera : public image{
private:
	int Width;          //JPEG�摜�f�[�^�̕�
	int Height;         //JPEG�摜�f�[�^�̍���
	unsigned char *nR;  //�Ԃ̗v�f�̃r�b�g�f�[�^
	unsigned char *nG;  //�΂̉�f�̃r�b�g�f�[�^
	unsigned char *nB;  //�̉�f�̃r�b�g�f�[�^
	int stop_flag;      //���摜��~�t���O
	
	int direction;      //�ύX����J�����̌���
	CQueryByMouseDlg* pParent;

	bmp input;  //�I�t���C���pBMP�N���X

	//cv::Mat outt;
	int isPointing, minx, miny, maxx, maxy;
	SOCKET sock_for_finger; // Socket for detect pointing

public:
	
	int SendCommand(int camera,int task);  //������𑗂�
	int OffLine();                         //�I�t���C���pBMP�t�@�C���̓ǂݍ���

	void set_stop(){ stop_flag=1; }        //���摜��~�t���O��ON�ɂ���
	
	void ChangeDirection(int course){ direction = course; }  //�ύX����J�����̌������Z�b�g
	
	void set(CQueryByMouseDlg* parent){pParent=parent;}
	bool get_finger(int xmin[],int ymin[],int xmax[],int ymax[]); //�w�����Ă���̈��Ԃ�

	void set_pParent(CQueryByMouseDlg* p){pParent = p;} //�e�E�B���h�E�ւ̃|�C���^�ݒ�
	//bool get_finger(int xmin[],int ymin[],int xmax[],int ymax[]); //�w�����Ă���̈��Ԃ�
	int imageKinect(CCamera * cp);

};