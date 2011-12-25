////-----------------
//// Header File
////-----------------
#include "stdafx.h"
//
//#include <iostream>
//#include <string>
//#include <cstring>
//#include <cmath>
//#include <fstream>
//#include "image.h"
//#include "bmp.h"
//using namespace std;
//
////#include "cv.h"
////#include "highgui.h"
//
//int tmp_frame=0;
//
///********/
///* 監視 */
///********/
//int image::watch(int frame)
//{
//	static bmp pre;       //推定した背景画像
//	bmp now;              //現在フレームの画像
//
//	tmp_frame=frame;
//
//	//現在の入力画像。輝度値のみ使用
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			now.Y_d(i,j)=(double)(R(i,j)*0.299+G(i,j)*0.587+B(i,j)*0.114);
//		}
//	}
//
//	//背景推定なし
///*	if(frame==0){
//		for(int j=0;j<Height;j++){
//			for(int i=0;i<Width;i++){
//				pre.Y_d(i,j)=now.Y_d(i,j);
//			}
//		}
//	}
//*/
//	//背景をM推定を用いて背景を推定し更新(毎フレーム更新)
//	M_estimator(pre,now);
//
//	if(frame<=60){
//		now.release();
//		track_flag=false;
//		return 0;
//	}
//
//	//推定した背景との差分
//	BackDiff(pre,now);
//
//#if 0 //肌色検出のみ
//	D_skincolor();
//	now.release();
//	return true;
//#endif
//
//	char file[30];  //ファイル名
//	char tmp[10];    //ファイル名用
//
//#if 0 //入力画像の保存
//	sprintf(file,"./Input/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	now.save_Y_d(file);
//#endif
//#if 0 //背景画像の保存
//	sprintf(file,"./Haikei/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	pre.save_Y(file);
//#endif
//#if 0 //差分画像の保存
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			now.isDiff(i,j)=isDiff(i,j);
//		}
//	}
//	sprintf(file,"./Sabun/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	now.save_isDiff(file);
//#endif
//
//	//人検出用
//	opening();                  //オープニング
//#if 0 //Opening結果の保存
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			now.isDiff(i,j)=isDiff(i,j);
//		}
//	}
//	sprintf(file,"./Opening/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	now.save_isDiff(file);
//#endif
//
//	closing();                  //クロージング
//#if 0 //Closing結果の保存
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			now.isDiff(i,j)=isDiff(i,j);
//		}
//	}
//	sprintf(file,"./Closing/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	now.save_isDiff(file);
//#endif
//
//	labeling(0,Width,0,Height); //ラベリング
//	Check_User();               //人検出
//
//#if 0 //差分などの最終結果出力用
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			if(isDiff(i,j)==NON)  isDiff(i,j)=0;
//			else if(isDiff(i,j)==HAND)	isDiff(i,j)=255;
//			else	isDiff(i,j)=120;
//		}
//	}
//#endif
//
//#if 0	//手のチェック
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			if(isDiff(i,j)==HAND){
//				B(i,j)=0; G(i,j)=255; R(i,j)=0;
//			}
//		}
//	}
//#endif
//
//	now.release();
//	
//	return 0;
//}
//
////------------------------------------------------------------------------
//// Name     : M_estimator(img &pre,img &now)
//// Author　 : Katayama Noriaki (CV-lab.)
//// Update   : 2005.8.19
//// function : ロバストと統計を用いて背景を推定する(濃淡画像のみに対応)
//// Argument : image型の背景画像
//// return   : なし
////------------------------------------------------------------------------
//void image::M_estimator(image &pre,image &now)
//{
//	//const double eta = 0.4;             //固定したの適応率ηの値
//	const double a = 0.8;                //累計誤差の記憶率α 現在0.8で固定
//	double eta;                          //環境の変化に対する適応率
//	const double eta_min = 0.001;        //適応率の最小値
//	long int M = Height*Width; //画素数M
//
//	double Eps_x;                        //ある画素における入力画像と背景画像の輝度値の差
//	double psi_x;                        //ρの微分ψ(εx)
//	static bool flag = false;            //初期化フラグ
//	static long int counter;             //フレーム数カウント
//	double *dist = new double[M];       //輝度値の差の絶対値の二乗
//	int diff_count = 0;
//
//	++counter;//フレーム数をインクリメント
//
//	////ファイルに書き出す
//	//FILE *fo;
//	//if((fo = fopen("valuelog.txt", "a")) == NULL){
//	//	//::MessageBox(NULL,"出力ファイルオープン失敗しました---ログファイル",NULL,MB_OK);
//	//}
//
//	//初期化(１回のみ行う)背景画像の累計誤差Et=0,画素の輝度値全て128にする
//	if(flag == false){
//		for(int j=0; j<Height; j++){
//			for(int i=0; i<Width; i++){
//				pre.Y_d(i,j) = 128.0;
//				pre.Y(i,j)   = 128;
//				pre.E(i,j)   = 0.0;
//			}
//		}
//		flag = true;//フラグをオン
//	}
//
//	if(flag == true){//初期画像があれば
//
//#if 0//←１にしたらηの自動調整をする
//
//		double sigma_x,sigma_y;              //標準偏差σx,σy
//		int  ave_pre,ave_now;                //ある画像の輝度値の平均値
//		long int sum_Cov = 0;                //xとyそれぞれの平均と差の合計
//		int m = 0;                           //画素数(メジアンに使う用)
//		long int sum_pre = 0,sum_now = 0;    //ある画像の輝度値合計
//		long int sum_x = 0,sum_y = 0;        //自分の平均との差の合計値
//
//
//		//輝度値の差の二乗を求める
//		for(int j=0; j<imageHeight; j++){
//			for(int i=0; i<imageWidth; i++){
//				//差の絶対値の二乗を格納
//				dist[m] = fabs((double)(now.Y(i,j) - pre.Y(i,j))) 
//					* fabs((double)(now.Y(i,j) - pre.Y(i,j)));
//				if(dist[m] > 100) diff_count++;
//				m++;//画素数
//			}
//		}
//
//		//fprintf(fo,"差分の画素数 = %d \n",diff_count);
//
//
//		//#################################
//		//例外値を求める
//		double med;//メジアン(中央値)
//		double sigma_ber;//標準偏差の推定値
//		double Exception_val;//例外値
//		double di;//輝度値の差の絶対値の二乗の平方根
//		long int Excep_num = 0;//例外値ではない画素数
//
//		qsort(dist,0,m-1);//クイックソートを行う
//		med = dist[m/2];//メジアンを求める
//
//		//標準偏差の推定値を求める計算
//		sigma_ber = 1.4826 * (double)(1 + (5 / (m-1))) * sqrt(med);
//		//2.5*σを例外値とする
//		Exception_val = sigma_ber * 2.5;
//
//		//fprintf(fo,"例外値 = %f \n",Exception_val);
//
//		for(int j=0; j<imageHeight; j++){
//			for(int i=0; i<imageWidth; i++){
//				di =sqrt( fabs((double)(now.Y(i,j) - pre.Y(i,j))) 
//					* fabs((double)(now.Y(i,j) - pre.Y(i,j))));
//				if(Exception_val > di){
//					sum_pre += pre.Y(i,j);//背景画像の輝度値の合計
//					sum_now += now.Y(i,j);//入力画像の輝度値の合計
//					++Excep_num;
//				}
//			}
//		}
//
//		//fprintf(fo,"sum_pre->%d sum_now->%d\n",sum_pre,sum_now);	
//		ave_pre = sum_pre / Excep_num; //背景画像の平均値
//		ave_now = sum_now / Excep_num; //入力画像の平均値
//		//fprintf(fo,"pre->%d now->%d\n",ave_pre,ave_now);	
//
//		int diff_x,diff_y;//平均との差
//		double Cov_xy;//共分散
//		double r_rob;//ロバスト相関係数r_rob
//
//		for(int j=0; j<imageHeight; j++){
//			for(int i=0; i<imageWidth; i++){
//				di = sqrt(fabs((double)(now.Y(i,j) - pre.Y(i,j))) 
//					* fabs((double)(now.Y(i,j) - pre.Y(i,j))));
//				if(Exception_val > di){
//					diff_x = pre.Y(i,j) - ave_pre;
//					diff_y = now.Y(i,j) - ave_now;
//
//					//背景画像のある値の平均との差の二乗和
//					sum_x   += diff_x * diff_x;
//					//入力画像のある値の平均との差の二乗和
//					sum_y   += diff_y * diff_y;
//					//背景と入力画像の輝度値と平均の差を掛けたものの和
//					sum_Cov += diff_x * diff_y;
//				}
//			}
//		}
//
//		//背景と入力画像それぞれの標準偏差と共分散を求め
//		//ロバストテンプレートマッチングを行い相関係数を求める
//		sigma_x = sqrt((double)(sum_x / Excep_num));  //背景画像標準偏差  σx
//		sigma_y = sqrt((double)(sum_y / Excep_num));  //入力画像の標準偏差σy
//		Cov_xy  = (double)(sum_Cov / Excep_num);       //xとyの共分散
//		r_rob   = Cov_xy / (sigma_x * sigma_y);  //相関係数r_robを求める
//
//		//fprintf(fo,"σx=%f ",sigma_x);
//		//fprintf(fo,"σy=%f\n",sigma_y);
//		//fprintf(fo,"共分散%f\n",Cov_xy);
//		//fprintf(fo,"%f\n",r_rob);
//
//		//相関値r -1 ≦ r ≦ 1 に抑える
//		if(sigma_x == 0.0 || sigma_y == 0.0) r_rob = 0.0;
//
//		//適応率η = f(r_rob) の自動調節
//		eta = 0.8 * (1.0 - r_rob);//f(r_rob) = 0.8(1-r_rob)
//		eta  = eta + eta_min;//少しの変化にも対応するためゲタを履かせておく
//
//		//fprintf(fo,"η = %f\n",eta);
//
//#endif//ηの自動調節のコメントアウトここまで
//
//		//η微調整
//		eta = 0.001;
//		if(counter < 60) eta = 0.9;
//		if(diff_count > (M / 10)) eta = 0.8;
//
//		//##############################################################
//		//ロバスト統計に基づき背景の輝度値を推定
//		//輝度値θ(theta)を推定し背景画像の値に代入
//
//		for(int j=0; j<Height; j++){
//			for(int i=0; i<Width; i++){
//				//輝度値の差ε(x) = 入力画像Xt - 背景画像θ
//				Eps_x = (now.Y_d(i,j) - pre.Y_d(i,j));
//
//				//ρの微分ψ(εx)
//				psi_x = tanh(Eps_x / 50);
//
//				//Etを最小とする背景の輝度値θtを最急降下法で求める
//				//[数式]θt = θt-1 -η(dEt / dθ)
//				pre.E(i,j) = psi_x + (a * pre.E(i,j));
//
//				//背景画像の輝度値更新
//				pre.Y_d(i,j) = pre.Y_d(i,j) + (eta * pre.E(i,j));
//
//				//unsigned char 型に値を戻す
//				pre.Y(i,j) = (unsigned char)pre.Y_d(i,j);
//
//				pre.R(i,j) = pre.G(i,j) = pre.B(i,j) = pre.Y(i,j);
//
//				//値の初期化
//				psi_x = Eps_x = 0.0;
//			}
//		}
//		//##############################################################
//	}
//
//	delete [] dist;
//	//fclose(fo);
//}
//
///************/
///* 背景差分 */
///************/
//void image::BackDiff(image &back,image &input)
//{
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			double difference = sqrt((back.Y_d(i,j)-input.Y_d(i,j))*(back.Y_d(i,j)-input.Y_d(i,j)));
//
//			if(difference<15){ //背景
//				isDiff(i,j) = NON;
//				isDiff(i,j) = NON;
//				isDiff(i,j) = NON;
//			}else{ //差分
//				isDiff(i,j) = BACK;
//				isDiff(i,j) = BACK;
//				isDiff(i,j) = BACK;
//			}
//		}//for
//	}//for
//}
//
///****************/
///* 人を検出する */
///****************/
/////////////////////////////////////////////////
////  3000画素以上のものを人候補とする
/////////////////////////////////////////////////
//bool image::Check_User()
//{
//	int *LUT;
//	int *H_label;
//	int P_num=0;   //人候補数
//	int H_num=0;   //手候補
//	static int count=0;
//	
//	LUT = new int[label];
//	H_label = new int[label];
//
//	//人候補を探す
//	for(int i=0;i<label;i++){
//		if((skin_num[i]>0) && (nlabel[i]<2000)){ //手候補
//			H_label[++H_num]=i;
//			continue;
//		}
//		if(nlabel[i]<3000){ //人、手候補以外
//			LUT[i] = 0;
//			continue;
//		}
//		LUT[i] = ++P_num;
//	}
//	label = P_num+1;    //ラベル数は、人候補数(+1)
//	h_label = H_num+1;  //手の候補数(+1)
//
//	for(int i=1;i<h_label;i++){
//		LUT[H_label[i]] = label+i;
//	}
//	
//	//人候補がなければ
//	if(P_num<=0){
//		count++;
//		if(count>10){
//			track_flag=false;  //追跡フラグをOff
//			pf=false;
//		}
//		delete [] LUT;
//		if(nlabel!=0)	delete [] nlabel;
//		if(skin_num!=0) delete [] skin_num;
//		delete [] p;
//		delete [] H_label;
//		return false;
//	}
//	count=0;
//
//	//重心計算用
//	xcount = new int[label+h_label];
//	ycount = new int[label+h_label];
//	
//	//外接長方形の座標用の領域確保
//	xmin = new int[label+h_label];  xmax = new int[label+h_label];
//	ymin = new int[label+h_label];  ymax = new int[label+h_label];
//
//	//初期化
//	for(int i=0;i<label+h_label;i++){
//		xcount[i] = 0;
//		ycount[i] = 0;
//		nlabel[i] = 0;
//		xmin[i] = Width;   xmax[i] = 0;
//		ymin[i] = Height;  ymax[i] = 0;
//	}
//	
//	//ラベル変更
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			if(p[j*Width+i]>0){
//				p[j*Width+i] = LUT[p[j*Width+i]];
//				if(p[j*Width+i]==0){
//					isDiff(i,j)=NON;
//				}else{
//					xcount[p[j*Width+i]] += i;
//					ycount[p[j*Width+i]] += j;
//					nlabel[p[j*Width+i]]++;
//					if(xmin[p[j*Width+i]]>i) xmin[p[j*Width+i]] = i;
//					if(xmax[p[j*Width+i]]<i) xmax[p[j*Width+i]] = i;
//					if(ymin[p[j*Width+i]]>j) ymin[p[j*Width+i]] = j;
//					if(ymax[p[j*Width+i]]<j) ymax[p[j*Width+i]] = j;
//				}
//			}else{
//				isDiff(i,j)=NON;
//			}
//		}
//	}
//
//	//重心を求める
//	for(int i=1;i<label;i++){
//		xcount[i] = xcount[i]/nlabel[i];
//		ycount[i] = ycount[i]/nlabel[i];
//
//#if 0	//探索領域囲む
//		for(int j=ymin[i];j<ymax[i];j++){
//			B(xmin[i],j)=255; G(xmin[i],j)=0; R(xmin[i],j)=0;
//			B(xmax[i],j)=255; G(xmax[i],j)=0; R(xmax[i],j)=0;
//		}
//		for(int j=xmin[i];j<xmax[i];j++){
//			B(j,ymin[i])=255; G(j,ymin[i])=0; R(j,ymin[i])=0;
//			B(j,ymax[i])=255; G(j,ymax[i])=0; R(j,ymax[i])=0;
//		}
//#endif
//	}
//	
//	delete [] nlabel;
//	delete [] LUT;
//	delete [] p;
//	delete [] skin_num;
//	delete [] H_label;
//	
//	//顔や指先を見つける
//	if(Check_Skin()==false){
//		delete [] xmin;   delete [] xmax;
//		delete [] ymin;   delete [] ymax;
//		delete [] xcount; delete [] ycount;
//		return false;
//	}
//
//	delete [] xmin;   delete [] xmax;
//	delete [] ymin;   delete [] ymax;
//	delete [] xcount; delete [] ycount;
//	
//	return true;
//}
//
//
///********************/
///* 顔と指先を求める */
///********************/
///////////////////////////////////////////////////////////////
//// 顔は人全体の重心より上でラベル数が最大のものとする
//// 顔の重心より下の肌色領域を腕とする
//// 指先は顔の重心から最も遠いものとする
//// 指先は右と左両方探す
//////////////////////////////////////////////////////////////
//bool image::Check_Skin()
//{
//	int cfx,cfy; //顔の重心
//	bool FaceFlag=false;
//
//	int dist=0;            //各x座標の連続した肌色画素数
//	int Avg_dist=0;        //x座標の連続した肌色画素数の平均
//	int Skin_num=0;        //肌色検出されたx座標の数
//
//	int max_dist = INT_MIN;    //最大距離
//	int hand_x1=0,hand_y1=0;   //指先の座標(x1,y1)
//	int hand_x2=0,hand_y2=0;   //指先の座標(x2,y2)
//	int chand_x1=0,chand_y1=0; //手の重心
//	int chand_x2=0,chand_y2=0;
//	bool h_flag1 = false;
//	bool h_flag2 = false;
//	int xx=0,yy=0,g_count=0;
//
//	Person_num=0;   //画像中の人の数(初期化)
//
//	Finger=false;
//
//	for(int k=1;k<label;k++){
//		//前に顔が無かった場合
//		if(pf!=true){
//			if(DetectFace(k)==false){ //OpenCVで顔が検出されない場合
//				//顔検出(簡易版)
//				for(int j=ycount[k];j<ymax[k];j++){
//					for(int i=xmin[k];i<xmax[k];i++){
//						if(isDiff(i,j)==SKIN){
//							isDiff(i,j)=FACE;
//							xx += i;
//							yy += j;
//							++g_count;
//						}
//					}
//				}
//				//顔ラベルがなかった時を防ぐ
//				if(g_count>20){
//					cfx = xx/g_count;
//					cfy = yy/g_count;
//					FaceFlag=true;
//				}
//				xx=0; yy=0; g_count=0; //初期化
//			}else
//				FaceFlag=true;
//		}
//
//		if(pf==true){ //OpenCVで顔検出されている場合
//			//今は1人だけ判定
//			if(fxmax[k]>xmax[k]+50){
//				continue;
//			}
//
//			//探索領域を調整
//			if(fxmin[k]<xmin[k])	xmin[k]=fxmin[k]-20;
//			//else	xmin[k]=xmin[k]-30;
//			if(fxmax[k]>xmax[k])	xmax[k]=fxmax[k]+20;
//			//else	xmax[k]=xmax[k]+30;
//			if(fymin[k]<ymin[k])	ymin[k]=fymin[k]-20;
//			//else	ymin[k]=ymin[k]-20;
//			if(fymax[k]>ymax[k])	ymax[k]=fymax[k]+20;
//			//else	ymax[k]=ymax[k]+20;
//
//			if(xmin[k]<0)		xmin[k]=0;
//			if(xmax[k]>Width-1)	xmax[k]=Width-1;
//			if(ymin[k]<0)		ymin[k]=0;
//			if(ymax[k]>Height-1)ymax[k]=Height-1;
//
//#if 0		//ファイル出力
//			ofstream fout;
//			static int nn=1;
//			fout.open("info.txt",ios::app);
//			fout << "N0." << nn << "\n";
//			fout << "Tracking前" << "\n";
//			fout << "xmin：" << xmin[k] << "\t" << "xmax：" << xmax[k] << "\n";
//			fout << "ymin：" << ymin[k] << "\t" << "ymax：" << ymax[k] << "\n";
//			//fout << "fxmin：" << fxmin[k] << "\t" << "fxmax：" << fxmax[k] << "\n";
//			//fout << "fymin：" << fymin[k] << "\t" << "fymax：" << fymax[k] << "\n";
//			fout.close();
//			nn++;
//#endif
//
//			Tracking(k);  //トラッキング
//
//			//顔を囲む
//			for(int j=fymin[k];j<fymax[k];j++){
//				for(int i=0;i<3;i++){
//					if(fxmin[k]-i<0) break;
//					if(fxmax[k]+i>Width-1) break;
//					B(fxmin[k]-i,j)=0; G(fxmin[k]-i,j)=255; R(fxmin[k]-i,j)=0;
//					B(fxmin[k]+i,j)=0; G(fxmax[k]+i,j)=255; R(fxmax[k]+i,j)=0;
//				}
//			}
//			for(int j=fxmin[k];j<fxmax[k];j++){
//				for(int i=0;i<3;i++){
//					if(fymin[k]-i<0) break;
//					if(fymax[k]+i>Height-1) break;
//					B(j,fymin[k]-i)=0; G(j,fymin[k]-i)=255; R(j,fymin[k]-i)=0;
//					B(j,fymax[k]+i)=0; G(j,fymax[k]+i)=255; R(j,fymax[k]-i)=0;
//				}
//			}
//
//			//顔領域の設定
//			for(int j=fymin[k];j<fymax[k];j++){
//				for(int i=fxmin[k];i<fxmax[k];i++){
//					if(isDiff(i,j)==SKIN)
//						isDiff(i,j)=FACE;
//				}
//			}
//			
//			//顔の重心を求める
//			cfx = (fxmin[k]+fxmax[k])/2;
//			cfy = (fymin[k]+fymax[k])/2;
//			FaceFlag=true;
//		}// if(pf==true)
//
//		//指先を求める
//		if(FaceFlag==true){
//
//			//探索領域囲む
//			for(int j=ymin[k];j<ymax[k];j++){
//				B(xmin[k],j)=255; G(xmin[k],j)=0; R(xmin[k],j)=0;
//				B(xmax[k],j)=255; G(xmax[k],j)=0; R(xmax[k],j)=0;
//			}
//			for(int j=xmin[k];j<xmax[k];j++){
//				B(j,ymin[k])=255; G(j,ymin[k])=0; R(j,ymin[k])=0;
//				B(j,ymax[k])=255; G(j,ymax[k])=0; R(j,ymax[k])=0;
//			}
//
//			//手を捜す領域修正
//			if(ymin[k]<ycount[k]-50)	ymin[k]=ycount[k]-50;
//
//			//重心より下の肌色部分を手とする
//			for(int j=ymin[k];j<ycount[k];j++){
//				for(int i=xmin[k];i<xmax[k];i++){
//					if(isDiff(i,j)==SKIN)
//						isDiff(i,j)=HAND;
//				}
//			}
//
//			//ラベリングにより、手を探す
//			int value=0;
//			value=labeling_hand(xmin[k],xcount[k],ymin[k],ycount[k]);
//
//			int temp_x=0,temp_y=0;
//
//			if(value>0){
//				//手領域で一番遠い座標を求める
//				for(int j=ycount[k]-1;j>=ymin[k];j--){
//					for(int i=xmin[k];i<xcount[k]+15;i++){
//						if(p[j*Width+i]==value){
//							dist++;
//							xx+=i; yy+=j; ++g_count;
//							if(max_dist<PointDistance(i,j,cfx,cfy)){
//								max_dist=PointDistance(i,j,cfx,cfy);
//								hand_x1=i;
//								hand_y1=j;
//								h_flag1=true;
//							}
//							//右側の手領域に色をつける
//							//B(i,j)=255; G(i,j)=0; R(i,j)=0;
//						}else{
//							isDiff(i,j)=BACK;
//						}
//					}
//					if(Avg_dist==0){
//						Avg_dist=dist;
//						dist=0;
//						continue;
//					}
//					//if(dist>0 && dist<7 && dist<Avg_dist/4){ //指を指している
//					if(dist>0 && dist<Avg_dist/2){
//						h_flag1 =true;
//						Finger=true;
//						dist=0;
//						continue;
//					}
//					Avg_dist = (Avg_dist*Skin_num + dist)/(Skin_num+1); //平均を修正
//					if(dist>0)
//						Skin_num++;
//					dist=0;  //初期化
//				}
//				Avg_dist=0; Skin_num=0; //初期化
//			}else{
//				//手候補があれば
//				if(h_label>1){
//					for(int h=1;h<h_label;h++){
//						if(xmax[label+h]<xcount[k] && xmax[label+h]>xmin[k]-20 && ymax[label+h]<=ycount[k]+30){
//							for(int j=ymin[label+h];j<ymax[label+h];j++){
//								for(int i=xmin[label+h];i<xmax[label+h];i++){
//									if(isDiff(i,j)==SKIN){
//										isDiff(i,j)=HAND;
//										//右側の手領域に色をつける
//										//B(i,j)=255; G(i,j)=0; R(i,j)=0;
//										xx+=i; yy+=j; g_count++;
//										if(max_dist < PointDistance(i,j,cfx,cfy)){
//											max_dist = PointDistance(i,j,cfx,cfy);
//											hand_x1 = i;
//											hand_y1 = j;
//											h_flag1 = true;
//										}
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//
//			if(h_flag1==true){
//				//手の重心を求める
//				if(g_count>0){
//					chand_x1=xx/g_count;
//					chand_y1=yy/g_count;
//				}
//
//				//出力
//				for(int j=hand_y1-1;j<hand_y1+1;j++){
//					for(int i=1;i<2;i++){
//						if(hand_x1-i<0) break;
//						if(hand_x1+i>Width-1) break;
//						B(hand_x1-i,j)=0; G(hand_x1-i,j)=0; R(hand_x1-i,j)=255;
//						B(hand_x1+i,j)=0; G(hand_x1+i,j)=0; R(hand_x1+i,j)=255;
//					}
//				}
//				for(int j=hand_x1-1;j<hand_x1+1;j++){
//					for(int i=1;i<2;i++){
//						if(hand_y1-i<0) break;
//						if(hand_y1+i>Height-1) break;
//						B(j,hand_y1-i)=0; G(j,hand_y1-i)=0; R(j,hand_y1-i)=255;
//						B(j,hand_y1+i)=0; G(j,hand_y1+i)=0; R(j,hand_y1+i)=255;
//					}
//				}
//			  /*for(int j=chand_y1-2;j<chand_y1+2;j++){
//					for(int i=chand_x1-2;i<chand_x1+2;i++){
//						B(i,j)=0; G(i,j)=255; R(i,j)=0;
//					}
//				}*/
//
//				//指差し判定
//				if(xcount[k]-hand_x1<40 && Finger==false){
//					h_flag1=false;
//				}
//			}
//			delete [] p;
//
//			//初期化
//			max_dist = INT_MIN;
//			xx=0; yy=0; g_count=0; value=0;
//
//			value=labeling_hand(xcount[k],xmax[k],ymin[k],ycount[k]);
//
//			if(value>0){
//				//手領域で一番遠い座標を求める
//				//for(int j = ymin[k]; j < ycount[k]; j++){
//				for(int j=ycount[k]-1;j>=ymin[k];j--){
//					for(int i = xcount[k]+15; i < xmax[k]; i++){
//						if(p[j*Width+i]==value){
//							dist++;
//							xx+=i; yy+=j; g_count++;
//							if(max_dist < PointDistance(i,j,cfx,cfy)){
//								max_dist = PointDistance(i,j,cfx,cfy);
//								hand_x2 = i;
//								hand_y2 = j;
//								h_flag2 = true;
//							}
//							//左側の手領域に色をつける
//							//B(i,j)=0; G(i,j)=255; R(i,j)=0;
//						}else{
//							isDiff(i,j)=BACK;
//						}
//					}
//					if(Avg_dist==0){
//						Avg_dist=dist;
//						dist=0;
//						continue;
//					}
//					if(dist>0 && dist<Avg_dist/2){
//						h_flag2 =true;
//						Finger=true;
//						continue;
//					}
//					Avg_dist = (Avg_dist*Skin_num + dist)/(Skin_num+1); //平均を修正
//					if(dist>0)
//						Skin_num++;
//					dist=0;  //初期化
//				}
//			}else{
//				//手候補があれば
//				if(h_label>1){
//					for(int h=1;h<h_label;h++){
//						if(xcount[k]<xmin[label+h] && xmin[label+h]<xmax[k]+20 && ymax[label+h]<=ycount[k]+30){
//							for(int j=ymin[label+h];j<ymax[label+h];j++){
//								for(int i=xmin[label+h];i<xmax[label+h];i++){
//									if(isDiff(i,j)==SKIN){
//										isDiff(i,j)=HAND;
//										//左側の手領域に色をつける
//										//B(i,j)=0; G(i,j)=255; R(i,j)=0;
//										xx+=i; yy+=j; g_count++;
//										if(max_dist < PointDistance(i,j,cfx,cfy)){
//											max_dist = PointDistance(i,j,cfx,cfy);
//											hand_x2 = i;
//											hand_y2 = j;
//											h_flag2 = true;
//										}
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//
//			if(h_flag2==true){
//				//重心
//				if(g_count>0){
//					chand_x2=xx/g_count;
//					chand_y2=yy/g_count;
//				}
//				//出力
//				for(int j=hand_y2-1;j<hand_y2+1;j++){
//					for(int i=1;i<2;i++){
//						if(hand_x2-i<0) break;
//						if(hand_x2+i>Width-1) break;
//						B(hand_x2-i,j)=0; G(hand_x2-i,j)=0; R(hand_x2-i,j)=255;
//						B(hand_x2+i,j)=0; G(hand_x2+i,j)=0; R(hand_x2+i,j)=255;
//					}
//				}
//				for(int j=hand_x2-1;j<hand_x2+1;j++){
//					for(int i=1;i<2;i++){
//						if(hand_y2-i<0) break;
//						if(hand_y2+i>Height-1) break;
//						B(j,hand_y2-i)=0; G(j,hand_y2-i)=0; R(j,hand_y2-i)=255;
//						B(j,hand_y2+i)=0; G(j,hand_y2+i)=0; R(j,hand_y2+i)=255;
//					}
//				}
//
//				//for(int j=chand_y2-2;j<chand_y2+2;j++){
//				//	for(int i=chand_x2-2;i<chand_x2+2;i++){
//				//		B(i,j)=0; G(i,j)=255; R(i,j)=0;
//				//	}
//				//}
//
//				//指差し判定
//				if(hand_x2-xcount[k]<40 && Finger==false){
//					h_flag2=false;
//				}
//			}
//			delete [] p;
//
//#if 0		//ファイル出力
//			ofstream fout;
//			fout.open("info.txt",ios::app);
//			fout << "N0." << tmp_frame << "\n";
//			fout << "指差し" << "\n";
//			fout << "重心x：" << xcount[k] << "\t" << "重心y" << ycount[k] << "\n";
//			fout << "hand_x1：" << hand_x1 << "\t" << "hand_y1：" << hand_y1 << "\n";
//			fout << "hand_x2：" << hand_x2 << "\t" << "hand_y2：" << hand_y2 << "\n\n";
//			fout.close();
//#endif
//
//			static int count=10;  //指差し判定
//
//			//指差している場所を求める
//			int chand_x,chand_y;
//			int tmp_handx;
//			if(h_flag1==false && h_flag2==false){  //両手とも指していない場合
//				count++;
//				if(count>=10){
//					Finger=false;
//					count=10;
//				}else{
//					Finger=true;
//				}
//				continue;
//			}else if(h_flag1==true && h_flag2==true){  //両手とも指している場合
//				if(PointDistance(hand_x1,hand_y1,cfx,cfy)>PointDistance(hand_x2,hand_y2,cfx,cfy)){
//					tmp_handx=hand_x1; ymax_point=hand_y1;
//					chand_x=chand_x1;	chand_y=chand_y1;
//					h_flag1=false; h_flag2=false;
//				}else{
//					tmp_handx=hand_x2; ymax_point=hand_y2;
//					chand_x=chand_x2;	chand_y=chand_y2;
//					h_flag1=false; h_flag2=false;
//				}
//			}else if(h_flag1==true){  //左側が指差している場合
//				tmp_handx=hand_x1; ymax_point=hand_y1;
//				chand_x=chand_x1;	chand_y=chand_y1;
//			}else if(h_flag2==true){  //右側が指指している場合
//				tmp_handx=hand_x2; ymax_point=hand_y2;
//				chand_x=chand_x2;	chand_y=chand_y2;
//			}
//			Finger=true;
//			count=0;
//
//			int r=15;
//			int circle_x=0;
//			double tan=((double)(chand_y-ymax_point)/(double)(chand_x-tmp_handx));
//			if(tan<-1.0 || tan>1.0)	r=15;
//			if(tan>=0){ //左側
//				circle_x=tmp_handx-r;
//				xmin_point=circle_x-r;
//				xmax_point=tmp_handx;
//			}else{   //右側
//				circle_x=tmp_handx+r;
//				xmin_point=tmp_handx;
//				xmax_point=circle_x+r;
//			}
//			//int circle_y = ymax_point-tan*(tmp_handx-circle_x);
//			//ymin_point=circle_y-(ymax_point-circle_y);
//			ymin_point=ymax_point-2*r;
//
//			//調整
//			if(xmax_point>Width-3)	xmax_point=Width-3;
//			if(ymin_point<2)		ymin_point=2;
//			if(xmin_point<2)		xmin_point=2;
//			if(ymax_point>Height-3)	ymax_point=Height-3;
//
//			//さしている場所を描く
//			for(int j=ymin_point;j<ymax_point;j++){
//				for(int i=0;i<2;i++){
//					B(xmin_point-i,j)=0; G(xmin_point-i,j)=0; R(xmin_point-i,j)=255;
//					B(xmax_point+i,j)=0; G(xmax_point+i,j)=0; R(xmax_point+i,j)=255;
//				}
//			}
//			for(int j=xmin_point;j<xmax_point;j++){
//				for(int i=0;i<2;i++){
//					B(j,ymin_point-i)=0; G(j,ymin_point-i)=0; R(j,ymin_point-i)=255;
//					B(j,ymax_point+i)=0; G(j,ymax_point+i)=0; R(j,ymax_point+i)=255;
//				}
//			}
//		}
//	}
//
//	return true;
//}
//
///**********************/
///* OpenCVによる顔検出 */
///**********************/
////bool image::DetectFace(int k)
////{
////	static int count=0;
////
////	static CvMemStorage* storage = 0;             //オブジェクト検出結果を蓄えておくメモリ
////	static CvHaarClassifierCascade* cascade  = 0; //stage classifierの木、すなわち、cascade
////	static CvHaarClassifierCascade* cascade1 = 0;
////
////	IplImage *img;         //OpenCV用画像データ構造体
////	
////	if(count==0){
////		//テンプレートの読み込み
////		cascade  = (CvHaarClassifierCascade*)cvLoad( "haarcascade_frontalface_alt.xml", 0, 0, 0 );
////		cascade1 = (CvHaarClassifierCascade*)cvLoad( "haarcascade_profileface.xml", 0, 0, 0 );
////
////		storage = cvCreateMemStorage(0);   //メモリ領域確保
////		count++;
////	}
////
////	//画像サイズの調整
////	while((xmax[k]-xmin[k])%4!=0)
////		xmin[k]--;
////	if(xmin[k]<0)
////		xmin[k] += 4;
////
////	//ヘッダの作成とデータ領域の確保
////	img = cvCreateImage( cvSize(xmax[k]-xmin[k],ymax[k]-ycount[k]),IPL_DEPTH_8U,3 );
////	img->origin = 1;
////
////	//画像データをimg(OpenCV用)に移す
////	int c=0;
////	for(int j=ycount[k];j<ymax[k];j++){
////		for(int i=xmin[k];i<xmax[k];i++){
////			img->imageData[c++] = B(i,j);
////			img->imageData[c++] = G(i,j);
////			img->imageData[c++] = R(i,j);
////		}
////	}
////
////	//とりあえず、保存して読み込み 最終的にはメモリ上で実現したい。
////	cvSaveImage("test.bmp",img);
////	cvReleaseImage( &img );
////
////	bool flag=false;
////
////	IplImage* im = cvLoadImage("test.bmp",1); //画像読み込み
////	
////	for(int j=0;j<=1;j++){ //顔検出(正面、横、上半身)
////		int scale = 1;
////		CvPoint pt1, pt2; //x座標とy座標を持つ構造体
////
////		cvClearMemStorage( storage );  //初期化？
////
////		//画像からオブジェクトを検出
////		CvSeq* faces;
////		if(j==0)
////			faces = cvHaarDetectObjects( im, cascade, storage,
////    		                                1.2, 1, CV_HAAR_DO_CANNY_PRUNING,
////        		                            cvSize(15,15) );
////		else if(j==1)
////			faces = cvHaarDetectObjects( im, cascade1, storage,
////    		                                1.2, 1, CV_HAAR_DO_CANNY_PRUNING,
////        		                            cvSize(15,15) );
////
////		for(int i = 0; i < (faces ? faces->total : 0); i++ ){
////			//顔の肌色チェックを行う
////    		CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
////    		pt1.x = r->x*scale;
////    		pt2.x = (r->x+r->width)*scale;
////    		pt1.y = r->y*scale;
////    		pt2.y = (r->y+r->height)*scale;
////			//cvRectangle( im, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
////			
////			//顔領域に対して、肌色チェック
////			int skin=0,total=0;
////			for(int l=ymax[k]-pt2.y;l<ymax[k]-pt1.y;l++){
////				for(int m=xmin[k]+pt1.x;m<xmin[k]+pt2.x;m++){
////					if(isDiff(m,l)==SKIN)
////						skin++;
////					total++;
////				}
////			}
////			//顔なら、座標を格納
////			if(skin>total/4){
//////				Person_num++;
////				fxmin[k]=xmin[k]+pt1.x; fxmax[k]=xmin[k]+pt2.x;
////				fymin[k]=ymax[k]-pt2.y; fymax[k]=ymax[k]-pt1.y;
////				flag = true;
////				//cvRectangle( im, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
////				break;    //今は人領域に顔一つ ←要修正
////			}
////		}//for(検出された顔のループ)
////		if(j<=1 && flag==true)
////			break;
////	}//for(顔検出のループ)
////
////	if(flag==false){
////		cvReleaseImage(&im);
////		pf=false;
////		return false;
////	}
////
////	pf=true;
////	cvReleaseImage( &im ); //領域解放
////	return true;
////}
//
///*********/
///*  追跡 */
///*********/
////bool image::Tracking(int k){
////	IplImage* img;
////
////	while((xmax[k]-xmin[k])%4!=0)
////		xmin[k]--;
////	if(xmin[k]<0)
////		xmin[k] += 4;
////
////	//ヘッダの作成とデータ領域の確保
////	img = cvCreateImage( cvSize(xmax[k]-xmin[k],ymax[k]-ycount[k]),IPL_DEPTH_8U,3 );
////	img->origin = 1;
////
////	//画像データをimg(OpenCV用)に移す
////	int c=0;
////	for(int j=ycount[k];j<ymax[k];j++){
////		for(int i=xmin[k];i<xmax[k];i++){
////			img->imageData[c++] = B(i,j);
////			img->imageData[c++] = G(i,j);
////			img->imageData[c++] = R(i,j);
////		}
////	}
////
////	cvSaveImage("test.bmp",img);
////	cvReleaseImage( &img );
////
////	IplImage* frame = cvLoadImage("test.bmp",1); //画像読み込み
////
////	static IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backproject = 0;
////	static CvHistogram *hist = 0;
////
////	static int track_object = 0;
////	CvRect selection;
////	static CvRect track_window;
////	CvConnectedComp track_comp;
////	int hdims = 16;
////	float hranges_arr[] = {0,180};
////	float* hranges = hranges_arr;
////	int vmin = 30, vmax = 256, smin = 70;
////
////	//テスト
////	if(track_flag==false){
////		selection.x=fxmin[k];
////		selection.y=Height-fymax[k];
////		selection.width=fxmax[k]-fxmin[k];
////		selection.height=fymax[k]-fymin[k];
////		track_object=-1;
////		track_flag=true;
////	}
////
////	//初めてなら、領域確保
////	if( !image ){
////		image = cvCreateImage( cvSize(320,240), 8, 3 );
////		cvZero(image);
////		image->origin = frame->origin;
////		hsv = cvCreateImage( cvGetSize(image), 8, 3 );
////		hue = cvCreateImage( cvGetSize(image), 8, 1 );
////		mask = cvCreateImage( cvGetSize(image), 8, 1 );
////		backproject = cvCreateImage( cvGetSize(image), 8, 1 );
////		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );  //ヒストグラムの作成
////	}
////
////	//imageにframeをコピー
////	c=0;
////	cvZero(image);
////	for(int j=Height-ymax[k];j<Height-ycount[k];j++){
////		for(int i=xmin[k];i<xmax[k];i++){
////			image->imageData[(j*320+i)*3]   = frame->imageData[c++];
////			image->imageData[(j*320+i)*3+1] = frame->imageData[c++];
////			image->imageData[(j*320+i)*3+2] = frame->imageData[c++];
////		}
////	}
////	
////    cvCvtColor( image, hsv, CV_BGR2HSV );  //RGBをHSVに変換(hsvに格納)
////
////	cvInRangeS( hsv, cvScalar(0,smin,vmin,0),
////                cvScalar(180,256,vmax,0), mask ); //2つのcvScalarの間の要素を見つける
////    
////    cvSplit( hsv, hue, 0, 0, 0 );   //hsvからHueだけを取り出す(hueに格納)
////
////	if( track_object < 0 ){           //領域を選択した直後
////		float max_val = 0.f;
////		cvSetImageROI( hue, selection );     //hueにselection(ROI)をセット
////		cvSetImageROI( mask, selection );    //maskにselectionをセット
////		cvCalcHist( &hue, hist, 0, mask );   //histにhueのヒストグラムを格納
////		cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );   //histの最大値を取得
////		cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 ); //変換？
////		cvResetImageROI( hue );     //hue(ROI)のクリア
////		cvResetImageROI( mask );    //maskのクリア
////		track_window = selection;   //cvRect型
////		track_object = 1;           //トラッキングを行う
////	}
////		
////	cvCalcBackProject( &hue, backproject, hist );  //バックプロジェクトの計算
////	cvAnd( backproject, mask, backproject, 0 );    //論理積
////	cvMeanShift(backproject,track_window,
////				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER,10,1),
////				&track_comp);     //探す
////	track_window = track_comp.rect;
////
////#if 0	//出力
////	for(int j=track_window.y;j<track_window.y+track_window.height;j++){
////		for(int i=0;i<3;i++){
////			image->imageData[(j*320+(track_window.x-i))*3]=0;
////			image->imageData[(j*320+(track_window.x-i))*3+1]=255;
////			image->imageData[(j*320+(track_window.x-i))*3+2]=0;
////			image->imageData[(j*320+(track_window.x+track_window.width+i))*3]=0;
////			image->imageData[(j*320+(track_window.x+track_window.width+i))*3+1]=255;
////			image->imageData[(j*320+(track_window.x+track_window.width+i))*3+2]=0;
////		}
////	}
////	for(int j=track_window.x;j<track_window.x+track_window.width;j++){
////		for(int i=0;i<3;i++){
////			image->imageData[((track_window.y-i)*320+j)*3]=0;
////			image->imageData[((track_window.y-i)*320+j)*3+1]=255;
////			image->imageData[((track_window.y-i)*320+j)*3+2]=0;
////			image->imageData[((track_window.y+track_window.height+i)*320+j)*3]=0;
////			image->imageData[((track_window.y+track_window.height+i)*320+j)*3+1]=255;
////			image->imageData[((track_window.y+track_window.height+i)*320+j)*3+2]=0;
////		}
////	}
////
////	char file[30];
////	char tmp[10];
////	static int q=0;
////	sprintf(file,"CV/");
////	sprintf(tmp,"%08lu",q++);
////	strcat(file,tmp);
////	strcat(file,".bmp");
////
////	cvSaveImage(file,image);
////#endif
////	fxmin[k]=track_window.x;	fymin[k]=Height-(track_window.y+track_window.height);
////	fxmax[k]=track_window.x+track_window.width;	fymax[k]=Height-track_window.y;
////
////	cvReleaseImage( &frame ); //領域解放
////
////	return true;
////}
//
///*******************/
///* 肌色検出(YCbCr) */
///*******************/
//void image::skincolor()
//{
//	int Yi, Cb, Cr;
//
//	/* RGBからYCbCrに変換 */
//	//for(int k=1;k<label;k++){
//		for(int j = 0; j < Height; j++){
//			for(int i = 0;i < Width; i++){
//				if(isDiff(i,j)==BACK){
//					Yi = (int)( 0.299  * R(i,j) + 0.587  * G(i,j) + 0.114  * B(i,j));
//					Cb = (int)(-0.1687 * R(i,j) - 0.3312 * G(i,j) + 0.5000 * B(i,j));
//					Cr = (int)( 0.5000 * R(i,j) - 0.4183 * G(i,j) - 0.0816 * B(i,j));
//		
//					//肌色以外(else)を緑にする
//					if( ((Yi > 20) && (Yi < 240))
//						&& ((Cb > -20) && (Cb < 20))
//						&& ((Cr > 10) && (Cr < 30)) ){
//							isDiff(i,j)=SKIN;
//					}
//					else{
//						//B(i,j) = 0; G(i,j)=255; R(i,j)=0;
//						//isDiff(i,j)=NON; //BACKのままでいい？
//					}
//				}
//			}//for
//		}//for
//	//}
//}
//
///*********************/
///* ラベリング(4近傍) */
///*********************/
//bool image::labeling(int Xmin,int Xmax,int Ymin,int Ymax)
//{
//	int *LUT;         // ルックアップテーブル
//
//	label=1; //新規ラベル
//
//	/* 領域確保 */
//	p = new int[Height*Width];
//	LUT = new int[Height*Width];
//
//	LUT[0] = 0;
//	
//	/* ラベル付け */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* 画素が端の場合 */
//			if(i==Ymin || i==Ymax-1 || j==Xmin || j==Xmax-1){
//				p[i*Width+j] = 0;
//				continue;
//			}
//			/* 画素が差分なら */
//			if(isDiff(j,i)!=NON){
//				/* 上、左の画素が共にラベル付けされていないなら */
//				if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = label;
//					LUT[label]=label;
//					label++;    // 新しいラベルを作る
//				}
//				/* 上の画素だけがラベル付けされているなら */
//				else if(p[(i-1)*Width+j]!=0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = p[(i-1)*Width+j];
//				}
//				/* 左の画素だけがラベル付けさているなら */
//				else if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]!=0){
//					p[i*Width+j] = p[i*Width+(j-1)];
//				}
//				/* 上、左の画素が共にラベル付けさているなら */
//				else{
//					/* 上の画素の方がラベルが小さい場合 */
//					if(p[(i-1)*Width+j]<=p[i*Width+(j-1)]){
//						p[i*Width+j] = p[(i-1)*Width+j];
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+(j-1)]] = p[i*Width+j];
//					}
//					/* 左の画素の方がラベルが小さい場合 */
//					else{
//						p[i*Width+j] = p[i*Width+(j-1)];
//						/* ラベルの等価性を変更 */
//						LUT[p[(i-1)*Width+j]] = p[i*Width+j];
//					}
//				}
//			} // if
//			/* 画素が0(黒)の場合 */
//			else{ 
//				p[i*Width+j] = 0;
//			}
//		} // for
//	} // for
//
//
//	/* 逆順にラベル付け */
//	for(int i=Ymax-2;i>Ymin;i--){
//		for(int j=Xmax-2;j>Xmin;j--){
//			if(p[i*Width+j]!=0){
//				/* 下の画素がラベル付けされているなら */
//				if(p[(i+1)*Width+j]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[(i+1)*Width+j]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[(i+1)*Width+j]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[(i+1)*Width+j]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+j]] = LUT[p[(i+1)*Width+j]];
//					}
//				}
//				/* 右の画素がラベル付けさているなら */
//				if(p[i*Width+(j+1)]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[i*Width+(j+1)]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+(j+1)]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[i*Width+(j+1)]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+j]] = LUT[p[i*Width+(j+1)]];
//					}
//				}
//			}// if
//		}// for
//	}// for
//	
//	/* 各ラベルの画素数を格納する領域の確保 */
//	nlabel = new int[label];
//	//体とはなれた手の部分の判定用
//	skin_num = new int[label];
//
//	/* 初期化 */
//	for(int i=0;i<label;i++){
//		nlabel[i]   = 0;
//		skin_num[i] = 0;
//	}
//	
//	/* ラベルの変更と画素数を数える */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* ラベル付けされている場合 */
//			if(p[i*Width+j] != 0){
//				/* ラベルを変更する必要がある場合、変更する */
//				while(p[i*Width+j] != LUT[p[i*Width+j]]){
//					p[i*Width+j] = LUT[p[i*Width+j]];
//				}
//				if(isDiff(j,i)==SKIN)	skin_num[p[i*Width+j]]++;
//				nlabel[p[i*Width+j]]++;
//			} // if
//		} // for
//	} // for
//
//	//体と離れた手の部分の判定
//	for(int i=1;i<label;i++){
//		if((nlabel[i]<50) || (skin_num[i]<(nlabel[i]*4/5))){
//			skin_num[i]=0;
//		}
//	}
//
//	/* 領域解放 */
//	delete [] LUT;
//
//	//delete [] skin_num;
//	
//	return true;
//}
//
///*********************/
///* ラベリング(4近傍) */
///*********************/
//int image::labeling_hand(int Xmin,int Xmax,int Ymin,int Ymax)
//{
//	int *LUT;         // ルックアップテーブル
//
//	int skin_label=1; //新規ラベル
//
//	/* 領域確保 */
//	p = new int[Height*Width];
//	LUT = new int[(Ymax-Ymin)*(Xmax-Xmin)];
//
//	LUT[0] = 0;
//	
//	/* ラベル付け */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* 画素が端の場合 */
//			if(i==Ymin || i==Ymax-1 || j==Xmin || j==Xmax-1){
//				p[i*Width+j] = 0;
//				continue;
//			}
//			/* 画素が肌色なら */
//			if(isDiff(j,i)==HAND){
//				/* 上、左の画素が共にラベル付けされていないなら */
//				if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = skin_label;
//					LUT[skin_label]=skin_label;
//					skin_label++;    // 新しいラベルを作る
//				}
//				/* 上の画素だけがラベル付けされているなら */
//				else if(p[(i-1)*Width+j]!=0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = p[(i-1)*Width+j];
//				}
//				/* 左の画素だけがラベル付けさているなら */
//				else if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]!=0){
//					p[i*Width+j] = p[i*Width+(j-1)];
//				}
//				/* 上、左の画素が共にラベル付けさているなら */
//				else{
//					/* 上の画素の方がラベルが小さい場合 */
//					if(p[(i-1)*Width+j]<=p[i*Width+(j-1)]){
//						p[i*Width+j] = p[(i-1)*Width+j];
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+(j-1)]] = p[i*Width+j];
//					}
//					/* 左の画素の方がラベルが小さい場合 */
//					else{
//						p[i*Width+j] = p[i*Width+(j-1)];
//						/* ラベルの等価性を変更 */
//						LUT[p[(i-1)*Width+j]] = p[i*Width+j];
//					}
//				}
//			} // if
//			/* 画素が0(黒)の場合 */
//			else{ 
//				p[i*Width+j] = 0;
//			}
//		} // for
//	} // for
//
//
//	/* 逆順にラベル付け */
//	for(int i=Ymax-2;i>Ymin;i--){
//		for(int j=Xmax-2;j>Xmin;j--){
//			if(p[i*Width+j]!=0){
//				/* 下の画素がラベル付けされているなら */
//				if(p[(i+1)*Width+j]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[(i+1)*Width+j]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[(i+1)*Width+j]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[(i+1)*Width+j]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+j]] = LUT[p[(i+1)*Width+j]];
//					}
//				}
//				/* 右の画素がラベル付けさているなら */
//				if(p[i*Width+(j+1)]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[i*Width+(j+1)]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+(j+1)]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[i*Width+(j+1)]]){
//						/* ラベルの等価性を変更 */
//						LUT[p[i*Width+j]] = LUT[p[i*Width+(j+1)]];
//					}
//				}
//			}// if
//		}// for
//	}// for
//	
//	/* 各ラベルの画素数を格納する領域の確保 */
//	nlabel = new int[skin_label];
//
//	/* 初期化 */
//	for(int i=0;i<skin_label;i++){
//		nlabel[i] = 0;
//	}
//	
//	/* ラベルの変更と画素数を数える */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* ラベル付けされている場合 */
//			if(p[i*Width+j] != 0){
//				/* ラベルを変更する必要がある場合、変更する */
//				while(p[i*Width+j] != LUT[p[i*Width+j]]){
//					p[i*Width+j] = LUT[p[i*Width+j]];
//				}
//				nlabel[p[i*Width+j]]++;
//			} // if
//		} // for
//	} // for
//
//	/* 最大領域を探す */
//	int max=0;
//	int value=0;
//	for(int i=0;i<skin_label;i++){
//		if(nlabel[i]>max){
//			max=nlabel[i];
//			value=i;
//		}
//	}
//
//	if(max<50)	value=0;
//
//	/* 領域解放 */
//	delete [] LUT;
//	delete [] nlabel;
//	
//	return value;
//}
//
////--------------------------------------------------------------
//// name:Get_Area(int xmin[],int ymin[],int xmax[],int ymax[])
//// 指差している領域を返す
//// xmin[]:領域のx座標の最小値 ymin[]:領域のy座標の最小値
//// xmax[]:領域のx座標の最大値 ymax[]:領域のy座標の最大値
////
////--------------------------------------------------------------
//bool image::Get_Area(int xmin[],int ymin[],int xmax[],int ymax[])
//{
//	if(Finger==true){
//		xmin[0] = xmin_point;
//		ymin[0] = ymin_point;
//		xmax[0] = xmax_point;
//		ymax[0] = ymax_point;
//		return true;
//	}
//	return false;
//}
//
///***************/
///* 2点間の距離 */
///***************/
//int image::PointDistance(const int a,const int b,const int c,const int d)
//{
//	int dist;
//
//	dist = (int)sqrt((double)(c - a) * (c - a) + (d - b) * ( d - b));
//
//	return dist;
//}
//
///****************/
///* オープニング */
///****************/
//void image::opening()
//{
//	int *tmp = new int[Width*Height];
//
//	skincolor();
//
//	//初期化
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			tmp[j*Width+i] = NON;
//		}
//	}
//
//	//収縮処理
//	for(int j=1;j<Height-2;j++) {
//		for(int i=1;i<Width-2;i++) {
//			if(isDiff(i,j)==SKIN){
//				tmp[j*Width+i]=SKIN;
//			}else if(isDiff(i,j)==BACK){  //差分が出て、肌色じゃない画素
//				int count=0;
//				for(int k=-1;k<2;k++){
//					for(int l=-1;l<2;l++){
//						if(isDiff(i+l,j+k)==NON) count++;
//					}
//				}
//				if(count>0) tmp[j*Width+i]=NON;
//				else tmp[j*Width+i]=BACK;
//			}
//		}
//	}
//
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			isDiff(i,j) = tmp[j*Width+i];
//		}
//	}
//
//	delete [] tmp;
//}
//
///****************/
///* クロージング */
///****************/
//void image::closing()
//{
//	int *tmp = new int[Width*Height];
//
//	for(int n=0;n<4;n++){
//		//初期化
//		for(int j=0;j<Height;j++){
//			for(int i=0;i<Width;i++){
//				tmp[j*Width+i] = NON;
//			}
//		}
//
//		//膨張処理
//		for(int j=2;j<Height-2;j++) {
//			for(int i=2;i<Width-2;i++) {
//				if(isDiff(i,j)==SKIN){
//					tmp[j*Width+i]=SKIN;
//				}else if(isDiff(i,j)==BACK){
//					int count=0;
//					for(int k=-1;k<3;k++){
//						for(int l=-1;l<3;l++){
//							if(isDiff(i+l,j+k)!=NON) count++;
//						}
//					}
//					if(count>0) tmp[j*Width+i]=BACK;
//					else tmp[j*Width+i]=NON;
//				}
//			}
//		}
//
//		for(int j=0;j<Height;j++){
//			for(int i=0;i<Width;i++){
//				isDiff(i,j) = tmp[j*Width+i];
//			}
//		}
//	}
//
//	delete [] tmp;
//}
//
///**********************/
///* デバック用肌色検出 */
///**********************/
//void image::D_skincolor()
//{
//	int Yi, Cb, Cr;
//
//	/* RGBからYCbCrに変換 */
//	for(int j = 0; j < Height; j++){
//		for(int i = 0;i < Width; i++){
//			//if(isDiff(i,j)==BACK){
//				Yi = (int)( 0.299  * R(i,j) + 0.587  * G(i,j) + 0.114  * B(i,j));
//				Cb = (int)(-0.1687 * R(i,j) - 0.3312 * G(i,j) + 0.5000 * B(i,j));
//				Cr = (int)( 0.5000 * R(i,j) - 0.4183 * G(i,j) - 0.0816 * B(i,j));
//
//#if 0		//ファイル出力
//			ofstream fout;
//			fout.open("info.txt",ios::app);
//			fout << "x:" << i << "\t" << "y:" << j << "\n";
//			fout << "Yi：" << Yi << "\t" << "Cb：" << Cb << "\t" << "Cr:" << Cr << "\n";
//			fout.close();
//#endif
//	
//				//肌色以外(else)を緑にする
//				//if( ((Yi > 20) && (Yi < 240))
//				//		&& ((Cb > -30) && (Cb < 20))
//				//		&& ((Cr > 10) && (Cr < 30)) ){
//				if( ((Yi > 20) && (Yi < 240))
//						&& ((Cb > -20) && (Cb < 20))
//						&& ((Cr > 10) && (Cr < 30)) ){
//							isDiff(i,j)=SKIN;
//						//isDiff(i,j)=SKIN;
//						if(isDiff(i,j)==BACK || isDiff(i,j)==SKIN){
//							B(i,j)=0; G(i,j)=0; R(i,j)=255;
//						}else{
//							B(i,j)=0; G(i,j)=255; R(i,j)=0;
//						}
//				}
//				else{
//					//B(i,j) = 0; G(i,j)=255; R(i,j)=0;
//					//isDiff(i,j)=NON; //BACKのままでいい？
//				}
//			//}
//		}//for
//	}//for
//}
//
///*********/
///* 2値化 */
///*********/
////bool image::blackWhite()
////{
////	unsigned char level;    // 画素の濃度値
////	unsigned int hist[256]; // 各濃度値の画素数
////	int threshold=0;        // 閾値
////	int n1,n2;              // 閾値未満、以上のクラスの画素数
////	unsigned long tmp;      // 閾値未満、以上のクラスの全ての画素の濃度の和
////	int sum=0,count=0;      // 全画素の濃度の和、全画素数
////	double ave;             // 全画素の平均濃度値
////	double ave1, ave2;      // 閾値未満、以上のクラスの平均濃度値
////	double var1,var2;       // 閾値未満、以上のクラスの分散の分子
////	double var_w;           // クラス内分散
////	double var_b;           // クラス間分散
////	double r;               // クラス内分散とクラス間分散の比
////	double max=-1.0;        // 判別比の最大値
////	int i,j,t;
////	
////	/* ヒストグラムの初期化 */
////	for(i=0;i<256;i++) {
////		hist[i] = 0;
////	}
////
////	/* ヒストグラムの作成 */
////	for(i=0;i<Height;i++) {
////		for(j=0;j<Width;j++) {
////			level = B(j,i);
////			hist[level] ++;
////			sum += B(j,i);
////			count++;
////		}
////	}
////	
////	/* 全画素の平均濃度値を求める */
////	ave = sum/count;
////
////	/* 閾値を0～255に変えながら、最適値を決定する */
////	for(t=0;t<=255;t++){
////		/* 初期化 */
////		n1 = n2 = 0;
////		ave1 = ave2 = 0;
////		var1 = var2 = 0;
////		tmp= 0;
////		
////		/* 閾値未満のクラス */
////		for( i = 0; i < t; i++ ) {
////			n1  += hist[i];
////			tmp += hist[i]*i;
////		}
////		/* 平均濃度値を求める */
////		if(n1!=0){
////			ave1 = (double)tmp / (double)n1;
////		}
////
////		/* 分散の分子を求める */
////		for(i=0;i<t;i++) {
////			var1 = var1 + (i-ave1)*(i-ave1)*hist[i];
////		}
////		/* 分散を求める */
////		if ( n1 != 0 ) {
////			var1 = var1/(double)n1;
////		}
////
////		tmp = 0; // 初期化
////		
////		/* 閾値以上のクラス */
////		for(i=t;i<=255;i++){
////			n2 = n2 + hist[i];
////			tmp = tmp + hist[i]*i;
////		}
////		/* 平均濃度値を求める */
////		if(n2!=0){ 
////			ave2 = (double)tmp / (double)n2;
////		}
////		/* 分散の分子を求める */
////		for(i=t;i<255;i++) {
////            var2 = var2 + (i-ave2)*(i-ave2)*hist[i];
////		}
////		/* 分散を求める */
////		if ( n2 != 0 ) { 
////			var2 = var2/(double)n2;
////		}
////
////		/* クラス内分散を求める */
////		var_w = (n1*var1 + n2*var2);	// 分母(n1+n2)は必要ないから省略
////		/* クラス間分散を求める */
////		var_b = n1*(ave1-ave)*(ave1-ave) + n2*(ave2-ave)*(ave2-ave);
////		
////		r  = var_b / var_w;
////		if (r>max){
////			max = r;
////			threshold = t;
////		}
////	}
////
////	cout << "2値化の閾値：" << threshold << '\n';
////	
////	/* 閾値未満なら0、閾値以上なら255 */
////	for(i=0;i<Height;i++){
////		for(j=0;j<Width;j++){
////			if(B(j,i)<threshold){
////				B(j,i) = 0;
////				G(j,i) = 0;
////				R(j,i) = 0;
////			}else{
////				B(j,i) = 255;
////				G(j,i) = 255;
////				R(j,i) = 255;
////			}
////		}
////	}
////	
////	return true;
////}
//
///**************/
///* エッジ検出 */
///**************/
////bool image::edgedetection()
////{
////	int *x,*y;
////	int min,max;
////	double p;
////	int i,j;
////	
////	/* 領域確保 */
////	x = new int[Height*Width];
////	y = new int[Height*Width];
////	
////	/* フィルタリング */
////	for(i=0;i<Height;i++){
////		for(j=0;j<Width;j++){
////			/* 画素が端なら */
////			if(i==0 || i==(Height-1) || j==0 || j==(Width-1)){
////				x[i*Width+j] = 0;
////				y[i*Width+j] = 0;
////			}else{
////				x[i*Width+j] = (-1)*B(j-1,i-1)+B(j+1,i-1)+(-2)*B(j-1,i)+2*B(j+1,i)+(-1)*B(j-1,i+1)+B(j+1,i+1);
////				y[i*Width+j] = (-1)*B(j-1,i-1)+(-2)*B(j,i-1)+(-1)*B(j+1,i-1)+B(j-1,i+1)+2*B(i+1,j)+B(j+1,i+1);
////			}
////		}
////	}
////	
////	/* 初期化 */
////	min = B(0,0);
////	max = B(0,0);
////	
////	/* 最小値、最大値を求める */
////	for(i=0;i<Height;i++){
////		for(j=0;j<Width;j++){
////			B(j,i) = (unsigned char)sqrt((double)(x[i*Width+j]*x[i*Width+j])+(double)(y[i*Width+j]*y[i*Width+j]));
////			G(j,i) = B(j,i);
////			R(j,i) = B(j,i);
////			if(min > B(j,i))
////				min = B(j,i);
////			if(B(i,j) > max)
////				max = B(j,i);
////		}
////	}
////	
////	/* スケール変換 */
////	if(min<0 || max>255){
////		p = 255.0 / (max - min);
////		for(i=0;i<Height;i++){
////			for(j=0;j<Width;j++){
////				B(j,i) = (unsigned char)(p*(B(j,i) - min));
////				G(j,i) = B(j,i);
////				R(j,i) = B(j,i);
////			}
////		}
////	}
////	
////	/* 領域解放 */
////	delete [] x;
////	delete [] y;
////	
////	return true;
////}