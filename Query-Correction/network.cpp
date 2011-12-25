//------------------------------------------------------------------------
// FileName : network.cpp
// Context  : ネットワークカメラにWinSockを使って接続し、カメラを制御
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
// Function : ネットワークカメラに文字列を送る
// Argument : camera / カメラの番号
//            task   / 実行する制御
// return   : int型の変数
//--------------------------------------------------
int CCamera::SendCommand(int camera,int task)
{
	SOCKET s;
	WSADATA wsaData;
	WORD wversion = WINSOCK_VERSION;
	SOCKADDR_IN saddr;
	char szBuf[20000];                     //受信データを格納
	int nRcv;                              //受信データのサイズ
	char szServer[3][20]={"133.19.22.237",
		                  "133.19.22.240",
						  "133.19.22.239"}; //Network Camera1～3のIPアドレス
	char szHost[20];       //自分のIPアドレス
	HOSTENT *lpHost;
	unsigned int addr;

	static unsigned long frame=0;   //フレーム数(何枚目のフレームか)

	//画像の幅の初期化(とりあえず、高さ・幅一定)
	Width = 320;
	Height = 240;

	//WinSockの初期化
	if(WSAStartup(wversion,&wsaData)){
		WSACleanup();
		_getch();
		return -1;
	}

	//Winsockeのバージョンをチェック
	if(wversion != wsaData.wVersion){
		return -1;
	}

	//ソケットオープン
	s = socket(AF_INET,SOCK_STREAM,0);
	if(s<0){
		WSACleanup();
		_getch();
		return -2;
	}

	//DNS名前解決（サーバーのIPアドレスを割り出す）
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

	//各種設定
	memset(&saddr,0,sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET; //アドレスファミリーを指定
	memcpy( &saddr.sin_addr, lpHost->h_addr, lpHost->h_length);//接続先IPアドレス指定（ここでは名前から)
	saddr.sin_port = htons(80); //接続先ポート番号指定(ここではポート番号80で固定)

	//自分のIP調べる
	char ac[80];
	if(gethostname(ac,sizeof(ac))==SOCKET_ERROR)	return 1;

	struct hostent *phe = gethostbyname(ac);
	if(phe==0)	return 1;

	struct in_addr haddr;
	memcpy(&haddr,phe->h_addr_list[0],sizeof(struct in_addr));
	strcpy_s(szHost,inet_ntoa(haddr));

	//Network Cameraに送る文字列を作る
	if(task==1){
		//カメラの向きを右に変える
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0607");
	}else if(task==2){
		//カメラの向きを左に変える
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0407");
	}else if(task==3){
		//カメラの向きを上に変える
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0807");
	}else if(task==4){
		//カメラの向きを下に変える
		sprintf_s(szBuf, "POST /command/ptzf.cgi HTTP/1.1\r\nHost:");
		strcat_s(szBuf,"szHost");
		strcat_s(szBuf,"\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nContent-Length: 13\r\n");
		strcat_s(szBuf, "\r\nrelative=0207");
	}else if(task==5){
		//動画像を取得
		/////////////////////////////////////////////////
		// interval:○msec間隔に画像を送る
		// number  :○枚画像を送る
		//
		// speed   :30が最大(25fps)
		////////////////////////////////////////////////

		//sprintf(szBuf,"GET /image?interval=40&number=200\r\nHost: "); 
		sprintf_s(szBuf,"GET /image?speed=30 HTTP/1.1\r\nHost: ");
		strcat_s(szBuf,szHost);
		strcat_s(szBuf,"\r\n\r\n");
	}

	//接続
	if(connect(s,(SOCKADDR *)&saddr,sizeof(saddr))<0){
		closesocket(s);
		WSACleanup();
		return -4;
	}

	//データを送信（要求）
	send(s,szBuf,(int)strlen(szBuf),0);
		
	//データを受信(1回目は画像データではない)
	nRcv = recv(s,szBuf,sizeof(szBuf),0);

	//動画像取得
	if(task==5){
		for( ; ( nRcv = recv( s, szBuf, sizeof(szBuf), 0)) > 0 ; ) {

			/*******************************/
			/* JPEGデータをRGBデータに変換 */
			/*******************************/

			CJpegDecoder jd;
			//JPEGデータの設定
			int r = jd.SetJpegData(szBuf,nRcv);
			//エラー?
			if(r<0)	break;

			//JPEGの復号
			r = jd.Decode();
			//エラー？
			if(r<0)	break;

			//RGBデータの取得(幅と高さも)
			jd.GetRGBData(&nR,&nG,&nB,Width,Height);

			//画像データ(RGB)の領域を確保(1番最初とカメラの向きを変えるとき)
			input.init();

			//RGBのデータを1つの配列に(上下を合わせる)
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

			//画像処理
			//Detection(frame);

			//画像出力
			pParent->ImageOut(&input.B(0,0));

			frame++;
			input.release();

			//フレームの初期化(1時間)
			/*if(frame==25*60*60){
				frame=1;
			}*/

			//カメラの向きを変える
			if(direction>0){
				SendCommand(camera,direction);
				direction=0;
				//frame=0;
			}

			//動画像取得終了
			if(stop_flag==1)  break;
		}//for
	}//if

	closesocket(s);  //ソケットクローズ
	WSACleanup();    //WinSockをクリーンアップ
	//getch();

	//preservation=0;  //保存フラグの初期化
	stop_flag=0;     //動画像停止フラグの初期化
	frame=0;         //フレーム番号の初期化

	return 0;
}

//--------------------------------------------------
// Name     : OffLine()
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2006.07.26
// Function : BMPファイルを読み込んで処理
// Argument : なし
// return   : int型の変数
//--------------------------------------------------
int CCamera::OffLine()
{
	unsigned long frame=0;   //フレーム数(何枚目のフレームか)

	//画像読み込み
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

	//	frame = i;  //フレーム番号更新
	//	sprintf_s(fname,"../../Capture4/");
	//	sprintf_s(tmp,"%08lu",i);
	//	strcat_s(fname,tmp);
	//	strcat_s(fname,".bmp");
	//	
	//	input.load(fname);
	//	//画像処理
	//	//Detection(frame);

	//	//画像出力
	//	//pParent->ImageOut(&input.B(0,0));

	//	input.release();
	//}

	input.init();

	return 0;
}


bool CCamera::get_finger(int xmin[],int ymin[],int xmax[],int ymax[])
{
	//if(Get_Area(xmin,ymin,xmax,ymax)==true){   //オンライン
	////if(Input.Get_Area(xmin,ymin,xmax,ymax)==true){ //オフライン
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
	char szBuf[32768];                     //受信データを格納
	int nRcv;                              //受信データのサイズ
	char szServer[20]={"133.19.23.174"}; //Network Camera1～3のIPアドレス
	char szHost[20]="133.19.23.79";         //自分のIPアドレス
	HOSTENT *lpHost;
	unsigned int addr;
	//u_short uport;

	//int frame=0;   //フレーム数(何枚目のフレームか)

	//WinSockの初期化
	if(WSAStartup(wversion,&wsaData)){
		WSACleanup();
		_getch();
		return -1;
	}

	//Winsockeのバージョンをチェック
	if(wversion != wsaData.wVersion){
		return -1;
	}

	//ソケットオープン
	s = socket(AF_INET,SOCK_STREAM,0);
	sock_for_finger = socket(AF_INET, SOCK_STREAM, 0);

	if( s < 0 || sock_for_finger < 0){
		WSACleanup();
		_getch();
		return -2;
	}


	//DNS名前解決（サーバーのIPアドレスを割り出す）
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

	//各種設定
	memset(&saddr,0,sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET; //アドレスファミリーを指定
	memcpy( &saddr.sin_addr, lpHost->h_addr, lpHost->h_length);//接続先IPアドレス指定（ここでは名前から)
	saddr.sin_port = htons(5000); //接続先ポート番号指定(ここではポート番号80で固定)
	//接続
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


	//データを送信（要求）
	//send(s,szBuf,(int)strlen(szBuf),0);

	//データを受信(1回目は画像データではない)
	//nRcv = recv(s,szBuf,sizeof(szBuf),0);

	char totalBuff[930000];
	int nTotalsize ;

	int sss = 640 * 480 * 3;
	int ivar ;//= (int*) malloc(sizeof(int));
	double dvar; //= (double*) malloc(sizeof(double));
	uchar* image_data= (uchar*) malloc(sizeof(uchar) * sss);
//	outt = cvCreateImage(cvSize(320, 240), 8, 3);
	//namedWindow("Image");

	//動画像取得

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
		//サイズをセット

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

	//解放
	//release_back();
	char quitSignal[] = "Quit";
	send(s,quitSignal, strlen(quitSignal), 0 );
	while (send(sock_for_finger, quitSignal, strlen(quitSignal), 0) < 0)
		cv::waitKey(10);
	closesocket(s);  //ソケットクローズ
	closesocket(sock_for_finger);
	WSACleanup();    //WinSockをクリーンアップ
	getch();

	stop_flag=0;     //動画像停止フラグの初期化

	return 0;
}