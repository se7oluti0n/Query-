//------------------------------------------------------------------------
// FileName : network.cpp
// Context  : �l�b�g���[�N�J������WinSock���g���Đڑ����A�J�����𐧌�
//
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2006.07.26
//------------------------------------------------------------------------
#include "stdafx.h"

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "JpegDecoder.h"
#include "network.h"
#include "cv.h"
#include "highgui.h"

using namespace cv;

//--------------------------------------------------
// Name     : SendCommand(int camera,int task)
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2006.07.26
// Function : �l�b�g���[�N�J�����ɕ�����𑗂�
// Argument : camera / �J�����̔ԍ�
//            task   / ���s���鐧��
// return   : int�^�̕ϐ�
//--------------------------------------------------
int CCamera::SendCommand(int camera,int task)
{
	SOCKET s;
	WSADATA wsaData;
	WORD wversion = WINSOCK_VERSION;
	SOCKADDR_IN saddr;
	char szBuf[20000];                     //��M�f�[�^���i�[
	int nRcv;                              //��M�f�[�^�̃T�C�Y
	char szServer[3][20]={"133.19.22.237",
		                  "133.19.22.240",
						  "133.19.22.239"}; //Network Camera1�`3��IP�A�h���X
	char szHost[20];       //������IP�A�h���X
	HOSTENT *lpHost;
	unsigned int addr;

	static unsigned long frame=0;   //�t���[����(�����ڂ̃t���[����)

	//�摜�̕��̏�����(�Ƃ肠�����A�����E�����)
	Width = 320;
	Height = 240;

	//WinSock�̏�����
	if(WSAStartup(wversion,&wsaData)){
		WSACleanup();
		_getch();
		return -1;
	}

	//Winsocke�̃o�[�W�������`�F�b�N
	if(wversion != wsaData.wVersion){
		return -1;
	}

	//�\�P�b�g�I�[�v��
	s = socket(AF_INET,SOCK_STREAM,0);
	if(s<0){
		WSACleanup();
		_getch();
		return -2;
	}

	//DNS���O�����i�T�[�o�[��IP�A�h���X������o���j
	lpHost = gethostbyname(szServer[camera-1]);
	if(lpHost == NULL){
		addr = inet_addr(szServer[camera-1]);
		lpHost = gethostbyaddr((char *)&addr,4,AF_INET);
	}
	if(lpHost == NULL){
		closesocket(s);
		WSACleanup();
		_getch();
		return -3;
	}

	//�e��ݒ�
	memset(&saddr,0,sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET; //�A�h���X�t�@�~���[���w��
	memcpy( &saddr.sin_addr, lpHost->h_addr, lpHost->h_length);//�ڑ���IP�A�h���X�w��i�����ł͖��O����)
	saddr.sin_port = htons(80); //�ڑ���|�[�g�ԍ��w��(�����ł̓|�[�g�ԍ�80�ŌŒ�)

	//������IP���ׂ�
	char ac[80];
	if(gethostname(ac,sizeof(ac))==SOCKET_ERROR)	return 1;

	struct hostent *phe = gethostbyname(ac);
	if(phe==0)	return 1;

	struct in_addr haddr;
	memcpy(&haddr,phe->h_addr_list[0],sizeof(struct in_addr));
	strcpy_s(szHost,inet_ntoa(haddr));

	//Network Camera�ɑ��镶��������
	if(task==1){
		//�J�����̌������E�ɕς���
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0607");
	}else if(task==2){
		//�J�����̌��������ɕς���
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0407");
	}else if(task==3){
		//�J�����̌�������ɕς���
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0807");
	}else if(task==4){
		//�J�����̌��������ɕς���
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0207");
	}else if(task==5){
		//���摜���擾
		/////////////////////////////////////////////////
		// interval:��msec�Ԋu�ɉ摜�𑗂�
		// number  :�����摜�𑗂�
		//
		// speed   :30���ő�(25fps)
		////////////////////////////////////////////////

		//sprintf(szBuf,"GET /image?interval=40&number=200\r\nHost: "); 
		sprintf_s(szBuf,"GET /image?speed=30 HTTP/1.1\r\nHost: ");
		strcat_s(szBuf,szHost);
		strcat_s(szBuf,"\r\n\r\n");
	}

	//�ڑ�
	if(connect(s,(SOCKADDR *)&saddr,sizeof(saddr))<0){
		closesocket(s);
		WSACleanup();
		return -4;
	}

	//�f�[�^�𑗐M�i�v���j
	send(s,szBuf,(int)strlen(szBuf),0);
		
	//�f�[�^����M(1��ڂ͉摜�f�[�^�ł͂Ȃ�)
	nRcv = recv(s,szBuf,sizeof(szBuf),0);

	//���摜�擾
	if(task==5){
		for( ; ( nRcv = recv( s, szBuf, sizeof(szBuf), 0)) > 0 ; ) {

			/*******************************/
			/* JPEG�f�[�^��RGB�f�[�^�ɕϊ� */
			/*******************************/

			CJpegDecoder jd;
			//JPEG�f�[�^�̐ݒ�
			int r = jd.SetJpegData(szBuf,nRcv);
			//�G���[?
			if(r<0)	break;

			//JPEG�̕���
			r = jd.Decode();
			//�G���[�H
			if(r<0)	break;

			//RGB�f�[�^�̎擾(���ƍ�����)
			jd.GetRGBData(&nR,&nG,&nB,Width,Height);

			//�摜�f�[�^(RGB)�̗̈���m��(1�ԍŏ��ƃJ�����̌�����ς���Ƃ�)
			input.init();

			//RGB�̃f�[�^��1�̔z���(�㉺�����킹��)
			if(camera!=2){
				for(int j=0;j<Height;j++){
					for(int i=0;i<Width;i++){
						input.B(i,Height-1-j) = nB[j*Width+i];
						input.G(i,Height-1-j) = nG[j*Width+i];
						input.R(i,Height-1-j) = nR[j*Width+i];
					}
				}
			}else{
				for(int j=0;j<Height;j++){
					for(int i=0;i<Width;i++){
						input.B(Width-1-i,j) = nB[j*Width+i];
						input.G(Width-1-i,j) = nG[j*Width+i];
						input.R(Width-1-i,j) = nR[j*Width+i];
					}
				}
			}

			if(nB) delete [] nB;
			if(nG) delete [] nG;
			if(nR) delete [] nR;

			//�摜����
			//Detection(frame);

			//�摜�o��
			pParent->ImageOut(&input.B(0,0));

			frame++;
			input.release();

			//�t���[���̏�����(1����)
			/*if(frame==25*60*60){
				frame=1;
			}*/

			//�J�����̌�����ς���
			if(direction>0){
				SendCommand(camera,direction);
				direction=0;
				//frame=0;
			}

			//���摜�擾�I��
			if(stop_flag==1)  break;
		}//for
	}//if

	closesocket(s);  //�\�P�b�g�N���[�Y
	WSACleanup();    //WinSock���N���[���A�b�v
	//getch();

	//preservation=0;  //�ۑ��t���O�̏�����
	stop_flag=0;     //���摜��~�t���O�̏�����
	frame=0;         //�t���[���ԍ��̏�����

	return 0;
}

//--------------------------------------------------
// Name     : OffLine()
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2006.07.26
// Function : BMP�t�@�C����ǂݍ���ŏ���
// Argument : �Ȃ�
// return   : int�^�̕ϐ�
//--------------------------------------------------
int CCamera::OffLine()
{
	unsigned long frame=0;   //�t���[����(�����ڂ̃t���[����)

	//�摜�ǂݍ���
	//char fname[30];
	//char tmp[10];

	while(1){
		//input.init();

		input.load("1.bmp");
	
		pParent->ImageOut(&input.B(0,0));

		input.release();

		if(stop_flag==1)	break;
	}

	//for(int i=0;i<800;i++){
	//	input.init();

	//	frame = i;  //�t���[���ԍ��X�V
	//	sprintf_s(fname,"../../Capture4/");
	//	sprintf_s(tmp,"%08lu",i);
	//	strcat_s(fname,tmp);
	//	strcat_s(fname,".bmp");
	//	
	//	input.load(fname);
	//	//�摜����
	//	//Detection(frame);

	//	//�摜�o��
	//	//pParent->ImageOut(&input.B(0,0));

	//	input.release();
	//}

	input.init();

	return 0;
}


bool CCamera::get_finger(int xmin[],int ymin[],int xmax[],int ymax[])
{
	//if(Get_Area(xmin,ymin,xmax,ymax)==true){   //�I�����C��
	////if(Input.Get_Area(xmin,ymin,xmax,ymax)==true){ //�I�t���C��
	//	return true;
	//}
	//return false
	
	char message_for_finger[] = "Get Finger";
	char recvBuff[256];
	int nRcv;



	send(sock_for_finger, message_for_finger, (int) strlen(message_for_finger), 0 );
	nRcv = recv(sock_for_finger, recvBuff, 256, 0);

	if (nRcv > 0){
		memcpy(&isPointing, recvBuff,					sizeof(int));
		memcpy(&minx,		recvBuff + sizeof(int) ,	sizeof(int));
		memcpy(&miny,		recvBuff + sizeof(int) * 2, sizeof(int));
		memcpy(&maxx,		recvBuff + sizeof(int) * 3, sizeof(int));
		memcpy(&maxy,		recvBuff + sizeof(int) * 4, sizeof(int));
	}


	if (isPointing == 1) {
		*xmax = maxx / 2;
		*xmin = minx / 2;
		*ymin = 240 - maxy / 2;
		*ymax = 240 - miny / 2;
		return true;
	}
	else
		return false;
}
int CCamera::imageKinect(CCamera * cp)
{
	SOCKET s;
	WSADATA wsaData;
	WORD wversion = WINSOCK_VERSION;
	SOCKADDR_IN saddr;
	char szBuf[32768];                     //��M�f�[�^���i�[
	int nRcv;                              //��M�f�[�^�̃T�C�Y
	char szServer[20]={"133.19.23.174"}; //Network Camera1�`3��IP�A�h���X
	char szHost[20]="133.19.23.79";         //������IP�A�h���X
	HOSTENT *lpHost;
	unsigned int addr;
	//u_short uport;

	//int frame=0;   //�t���[����(�����ڂ̃t���[����)

	//WinSock�̏�����
	if(WSAStartup(wversion,&wsaData)){
		WSACleanup();
		_getch();
		return -1;
	}

	//Winsocke�̃o�[�W�������`�F�b�N
	if(wversion != wsaData.wVersion){
		return -1;
	}

	//�\�P�b�g�I�[�v��
	s = socket(AF_INET,SOCK_STREAM,0);
	sock_for_finger = socket(AF_INET, SOCK_STREAM, 0);

	if( s < 0 || sock_for_finger < 0){
		WSACleanup();
		_getch();
		return -2;
	}


	//DNS���O�����i�T�[�o�[��IP�A�h���X������o���j
	lpHost = gethostbyname(szServer);
	if(lpHost == NULL){
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char *)&addr,4,AF_INET);
	}
	if(lpHost == NULL){
		closesocket(s);
		WSACleanup();
		_getch();
		return -3;
	}

	//�e��ݒ�
	memset(&saddr,0,sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET; //�A�h���X�t�@�~���[���w��
	memcpy( &saddr.sin_addr, lpHost->h_addr, lpHost->h_length);//�ڑ���IP�A�h���X�w��i�����ł͖��O����)
	saddr.sin_port = htons(5000); //�ڑ���|�[�g�ԍ��w��(�����ł̓|�[�g�ԍ�80�ŌŒ�)
	//�ڑ�
	if(connect(s,(SOCKADDR *)&saddr,sizeof(saddr))<0){
		closesocket(s);
		WSACleanup();
		return -4;
	}

	cp->isPointing = 0;
	// Connect to Pointing server
	saddr.sin_port = htons(5001);

	if (connect(sock_for_finger, (SOCKADDR *) &saddr, sizeof(saddr)) < 0){
		closesocket(sock_for_finger);
		WSACleanup();
		return -4;
	}


	//�f�[�^�𑗐M�i�v���j
	//send(s,szBuf,(int)strlen(szBuf),0);

	//�f�[�^����M(1��ڂ͉摜�f�[�^�ł͂Ȃ�)
	//nRcv = recv(s,szBuf,sizeof(szBuf),0);

	char totalBuff[930000];
	int nTotalsize ;

	int sss = 640 * 480 * 3;
	int ivar ;//= (int*) malloc(sizeof(int));
	double dvar; //= (double*) malloc(sizeof(double));
	uchar* image_data= (uchar*) malloc(sizeof(uchar) * sss);
//	outt = cvCreateImage(cvSize(320, 240), 8, 3);
	//namedWindow("Image");

	//���摜�擾

	while(1){
		nTotalsize = 0;

		while ((nRcv=recv(s,szBuf,sizeof(szBuf),0))	>	0)
		{
			// cout << "Recieved " << bytes_recieved << " bytes" << endl;
			if ( nTotalsize + nRcv > 930000 )
				break;
			memcpy( totalBuff + nTotalsize, szBuf, nRcv);
			nTotalsize +=nRcv;
			if ( totalBuff[nTotalsize - 1] == 'm' &&  totalBuff[nTotalsize - 2] == 'a' &&
				totalBuff[nTotalsize - 3] == 'n' &&  totalBuff[nTotalsize - 4] == 'h')
				break;



		}

		if (nTotalsize != 921616) continue; 
		//�T�C�Y���Z�b�g

		memcpy(image_data, totalBuff, sss);
		memcpy(&ivar, totalBuff + sss, sizeof(int));
		memcpy(&dvar, totalBuff + sss + sizeof(int), sizeof(double));

		cv::Mat img(cv::Size(640, 480), 16, image_data);
		cv::resize(img,img, cv::Size(320, 240));
		if (cp->isPointing)
			cv::rectangle(img, cv::Point(cp->minx / 2, cp->miny / 2), cv::Point(cp->maxx / 2, cp->maxy / 2), cv::Scalar(255, 0, 0));
		//memcpy(outt->imageData, img.data, 320 * 240 * 3 );
		cv::imshow("Movie", img);
		//cvShowImage("Image", outt);

		cout << "var i = " << ivar << " var double = " << dvar << endl;
		if (cv::waitKey(30) == 'q')
		{
			//closesocket(s);
			break;
		}

		//		::ReleaseDC(hwndStill,hdcStill);
	}//if

	//���
	//release_back();
	char quitSignal[] = "Quit";
	send(s,quitSignal, strlen(quitSignal), 0 );
	while (send(sock_for_finger, quitSignal, strlen(quitSignal), 0) < 0)
		cv::waitKey(10);
	closesocket(s);  //�\�P�b�g�N���[�Y
	closesocket(sock_for_finger);
	WSACleanup();    //WinSock���N���[���A�b�v
	getch();

	stop_flag=0;     //���摜��~�t���O�̏�����

	return 0;
}