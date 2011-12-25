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
///* �Ď� */
///********/
//int image::watch(int frame)
//{
//	static bmp pre;       //���肵���w�i�摜
//	bmp now;              //���݃t���[���̉摜
//
//	tmp_frame=frame;
//
//	//���݂̓��͉摜�B�P�x�l�̂ݎg�p
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			now.Y_d(i,j)=(double)(R(i,j)*0.299+G(i,j)*0.587+B(i,j)*0.114);
//		}
//	}
//
//	//�w�i����Ȃ�
///*	if(frame==0){
//		for(int j=0;j<Height;j++){
//			for(int i=0;i<Width;i++){
//				pre.Y_d(i,j)=now.Y_d(i,j);
//			}
//		}
//	}
//*/
//	//�w�i��M�����p���Ĕw�i�𐄒肵�X�V(���t���[���X�V)
//	M_estimator(pre,now);
//
//	if(frame<=60){
//		now.release();
//		track_flag=false;
//		return 0;
//	}
//
//	//���肵���w�i�Ƃ̍���
//	BackDiff(pre,now);
//
//#if 0 //���F���o�̂�
//	D_skincolor();
//	now.release();
//	return true;
//#endif
//
//	char file[30];  //�t�@�C����
//	char tmp[10];    //�t�@�C�����p
//
//#if 0 //���͉摜�̕ۑ�
//	sprintf(file,"./Input/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	now.save_Y_d(file);
//#endif
//#if 0 //�w�i�摜�̕ۑ�
//	sprintf(file,"./Haikei/");
//	sprintf(tmp,"%08lu",frame);
//	strcat(file,tmp);
//	strcat(file,".bmp");
//	pre.save_Y(file);
//#endif
//#if 0 //�����摜�̕ۑ�
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
//	//�l���o�p
//	opening();                  //�I�[�v�j���O
//#if 0 //Opening���ʂ̕ۑ�
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
//	closing();                  //�N���[�W���O
//#if 0 //Closing���ʂ̕ۑ�
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
//	labeling(0,Width,0,Height); //���x�����O
//	Check_User();               //�l���o
//
//#if 0 //�����Ȃǂ̍ŏI���ʏo�͗p
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			if(isDiff(i,j)==NON)  isDiff(i,j)=0;
//			else if(isDiff(i,j)==HAND)	isDiff(i,j)=255;
//			else	isDiff(i,j)=120;
//		}
//	}
//#endif
//
//#if 0	//��̃`�F�b�N
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
//// Author�@ : Katayama Noriaki (CV-lab.)
//// Update   : 2005.8.19
//// function : ���o�X�g�Ɠ��v��p���Ĕw�i�𐄒肷��(�Z�W�摜�݂̂ɑΉ�)
//// Argument : image�^�̔w�i�摜
//// return   : �Ȃ�
////------------------------------------------------------------------------
//void image::M_estimator(image &pre,image &now)
//{
//	//const double eta = 0.4;             //�Œ肵���̓K�����ł̒l
//	const double a = 0.8;                //�݌v�덷�̋L������ ����0.8�ŌŒ�
//	double eta;                          //���̕ω��ɑ΂���K����
//	const double eta_min = 0.001;        //�K�����̍ŏ��l
//	long int M = Height*Width; //��f��M
//
//	double Eps_x;                        //�����f�ɂ�������͉摜�Ɣw�i�摜�̋P�x�l�̍�
//	double psi_x;                        //�ς̔�����(��x)
//	static bool flag = false;            //�������t���O
//	static long int counter;             //�t���[�����J�E���g
//	double *dist = new double[M];       //�P�x�l�̍��̐�Βl�̓��
//	int diff_count = 0;
//
//	++counter;//�t���[�������C���N�������g
//
//	////�t�@�C���ɏ����o��
//	//FILE *fo;
//	//if((fo = fopen("valuelog.txt", "a")) == NULL){
//	//	//::MessageBox(NULL,"�o�̓t�@�C���I�[�v�����s���܂���---���O�t�@�C��",NULL,MB_OK);
//	//}
//
//	//������(�P��̂ݍs��)�w�i�摜�̗݌v�덷Et=0,��f�̋P�x�l�S��128�ɂ���
//	if(flag == false){
//		for(int j=0; j<Height; j++){
//			for(int i=0; i<Width; i++){
//				pre.Y_d(i,j) = 128.0;
//				pre.Y(i,j)   = 128;
//				pre.E(i,j)   = 0.0;
//			}
//		}
//		flag = true;//�t���O���I��
//	}
//
//	if(flag == true){//�����摜�������
//
//#if 0//���P�ɂ�����ł̎�������������
//
//		double sigma_x,sigma_y;              //�W���΍���x,��y
//		int  ave_pre,ave_now;                //����摜�̋P�x�l�̕��ϒl
//		long int sum_Cov = 0;                //x��y���ꂼ��̕��ςƍ��̍��v
//		int m = 0;                           //��f��(���W�A���Ɏg���p)
//		long int sum_pre = 0,sum_now = 0;    //����摜�̋P�x�l���v
//		long int sum_x = 0,sum_y = 0;        //�����̕��ςƂ̍��̍��v�l
//
//
//		//�P�x�l�̍��̓������߂�
//		for(int j=0; j<imageHeight; j++){
//			for(int i=0; i<imageWidth; i++){
//				//���̐�Βl�̓����i�[
//				dist[m] = fabs((double)(now.Y(i,j) - pre.Y(i,j))) 
//					* fabs((double)(now.Y(i,j) - pre.Y(i,j)));
//				if(dist[m] > 100) diff_count++;
//				m++;//��f��
//			}
//		}
//
//		//fprintf(fo,"�����̉�f�� = %d \n",diff_count);
//
//
//		//#################################
//		//��O�l�����߂�
//		double med;//���W�A��(�����l)
//		double sigma_ber;//�W���΍��̐���l
//		double Exception_val;//��O�l
//		double di;//�P�x�l�̍��̐�Βl�̓��̕�����
//		long int Excep_num = 0;//��O�l�ł͂Ȃ���f��
//
//		qsort(dist,0,m-1);//�N�C�b�N�\�[�g���s��
//		med = dist[m/2];//���W�A�������߂�
//
//		//�W���΍��̐���l�����߂�v�Z
//		sigma_ber = 1.4826 * (double)(1 + (5 / (m-1))) * sqrt(med);
//		//2.5*�Ђ��O�l�Ƃ���
//		Exception_val = sigma_ber * 2.5;
//
//		//fprintf(fo,"��O�l = %f \n",Exception_val);
//
//		for(int j=0; j<imageHeight; j++){
//			for(int i=0; i<imageWidth; i++){
//				di =sqrt( fabs((double)(now.Y(i,j) - pre.Y(i,j))) 
//					* fabs((double)(now.Y(i,j) - pre.Y(i,j))));
//				if(Exception_val > di){
//					sum_pre += pre.Y(i,j);//�w�i�摜�̋P�x�l�̍��v
//					sum_now += now.Y(i,j);//���͉摜�̋P�x�l�̍��v
//					++Excep_num;
//				}
//			}
//		}
//
//		//fprintf(fo,"sum_pre->%d sum_now->%d\n",sum_pre,sum_now);	
//		ave_pre = sum_pre / Excep_num; //�w�i�摜�̕��ϒl
//		ave_now = sum_now / Excep_num; //���͉摜�̕��ϒl
//		//fprintf(fo,"pre->%d now->%d\n",ave_pre,ave_now);	
//
//		int diff_x,diff_y;//���ςƂ̍�
//		double Cov_xy;//�����U
//		double r_rob;//���o�X�g���֌W��r_rob
//
//		for(int j=0; j<imageHeight; j++){
//			for(int i=0; i<imageWidth; i++){
//				di = sqrt(fabs((double)(now.Y(i,j) - pre.Y(i,j))) 
//					* fabs((double)(now.Y(i,j) - pre.Y(i,j))));
//				if(Exception_val > di){
//					diff_x = pre.Y(i,j) - ave_pre;
//					diff_y = now.Y(i,j) - ave_now;
//
//					//�w�i�摜�̂���l�̕��ςƂ̍��̓��a
//					sum_x   += diff_x * diff_x;
//					//���͉摜�̂���l�̕��ςƂ̍��̓��a
//					sum_y   += diff_y * diff_y;
//					//�w�i�Ɠ��͉摜�̋P�x�l�ƕ��ς̍����|�������̘̂a
//					sum_Cov += diff_x * diff_y;
//				}
//			}
//		}
//
//		//�w�i�Ɠ��͉摜���ꂼ��̕W���΍��Ƌ����U������
//		//���o�X�g�e���v���[�g�}�b�`���O���s�����֌W�������߂�
//		sigma_x = sqrt((double)(sum_x / Excep_num));  //�w�i�摜�W���΍�  ��x
//		sigma_y = sqrt((double)(sum_y / Excep_num));  //���͉摜�̕W���΍���y
//		Cov_xy  = (double)(sum_Cov / Excep_num);       //x��y�̋����U
//		r_rob   = Cov_xy / (sigma_x * sigma_y);  //���֌W��r_rob�����߂�
//
//		//fprintf(fo,"��x=%f ",sigma_x);
//		//fprintf(fo,"��y=%f\n",sigma_y);
//		//fprintf(fo,"�����U%f\n",Cov_xy);
//		//fprintf(fo,"%f\n",r_rob);
//
//		//���֒lr -1 �� r �� 1 �ɗ}����
//		if(sigma_x == 0.0 || sigma_y == 0.0) r_rob = 0.0;
//
//		//�K������ = f(r_rob) �̎�������
//		eta = 0.8 * (1.0 - r_rob);//f(r_rob) = 0.8(1-r_rob)
//		eta  = eta + eta_min;//�����̕ω��ɂ��Ή����邽�߃Q�^�𗚂����Ă���
//
//		//fprintf(fo,"�� = %f\n",eta);
//
//#endif//�ł̎������߂̃R�����g�A�E�g�����܂�
//
//		//�Ŕ�����
//		eta = 0.001;
//		if(counter < 60) eta = 0.9;
//		if(diff_count > (M / 10)) eta = 0.8;
//
//		//##############################################################
//		//���o�X�g���v�Ɋ�Â��w�i�̋P�x�l�𐄒�
//		//�P�x�l��(theta)�𐄒肵�w�i�摜�̒l�ɑ��
//
//		for(int j=0; j<Height; j++){
//			for(int i=0; i<Width; i++){
//				//�P�x�l�̍���(x) = ���͉摜Xt - �w�i�摜��
//				Eps_x = (now.Y_d(i,j) - pre.Y_d(i,j));
//
//				//�ς̔�����(��x)
//				psi_x = tanh(Eps_x / 50);
//
//				//Et���ŏ��Ƃ���w�i�̋P�x�l��t���ŋ}�~���@�ŋ��߂�
//				//[����]��t = ��t-1 -��(dEt / d��)
//				pre.E(i,j) = psi_x + (a * pre.E(i,j));
//
//				//�w�i�摜�̋P�x�l�X�V
//				pre.Y_d(i,j) = pre.Y_d(i,j) + (eta * pre.E(i,j));
//
//				//unsigned char �^�ɒl��߂�
//				pre.Y(i,j) = (unsigned char)pre.Y_d(i,j);
//
//				pre.R(i,j) = pre.G(i,j) = pre.B(i,j) = pre.Y(i,j);
//
//				//�l�̏�����
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
///* �w�i���� */
///************/
//void image::BackDiff(image &back,image &input)
//{
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			double difference = sqrt((back.Y_d(i,j)-input.Y_d(i,j))*(back.Y_d(i,j)-input.Y_d(i,j)));
//
//			if(difference<15){ //�w�i
//				isDiff(i,j) = NON;
//				isDiff(i,j) = NON;
//				isDiff(i,j) = NON;
//			}else{ //����
//				isDiff(i,j) = BACK;
//				isDiff(i,j) = BACK;
//				isDiff(i,j) = BACK;
//			}
//		}//for
//	}//for
//}
//
///****************/
///* �l�����o���� */
///****************/
/////////////////////////////////////////////////
////  3000��f�ȏ�̂��̂�l���Ƃ���
/////////////////////////////////////////////////
//bool image::Check_User()
//{
//	int *LUT;
//	int *H_label;
//	int P_num=0;   //�l��␔
//	int H_num=0;   //����
//	static int count=0;
//	
//	LUT = new int[label];
//	H_label = new int[label];
//
//	//�l����T��
//	for(int i=0;i<label;i++){
//		if((skin_num[i]>0) && (nlabel[i]<2000)){ //����
//			H_label[++H_num]=i;
//			continue;
//		}
//		if(nlabel[i]<3000){ //�l�A����ȊO
//			LUT[i] = 0;
//			continue;
//		}
//		LUT[i] = ++P_num;
//	}
//	label = P_num+1;    //���x�����́A�l��␔(+1)
//	h_label = H_num+1;  //��̌�␔(+1)
//
//	for(int i=1;i<h_label;i++){
//		LUT[H_label[i]] = label+i;
//	}
//	
//	//�l��₪�Ȃ����
//	if(P_num<=0){
//		count++;
//		if(count>10){
//			track_flag=false;  //�ǐՃt���O��Off
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
//	//�d�S�v�Z�p
//	xcount = new int[label+h_label];
//	ycount = new int[label+h_label];
//	
//	//�O�ڒ����`�̍��W�p�̗̈�m��
//	xmin = new int[label+h_label];  xmax = new int[label+h_label];
//	ymin = new int[label+h_label];  ymax = new int[label+h_label];
//
//	//������
//	for(int i=0;i<label+h_label;i++){
//		xcount[i] = 0;
//		ycount[i] = 0;
//		nlabel[i] = 0;
//		xmin[i] = Width;   xmax[i] = 0;
//		ymin[i] = Height;  ymax[i] = 0;
//	}
//	
//	//���x���ύX
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
//	//�d�S�����߂�
//	for(int i=1;i<label;i++){
//		xcount[i] = xcount[i]/nlabel[i];
//		ycount[i] = ycount[i]/nlabel[i];
//
//#if 0	//�T���̈�͂�
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
//	//���w���������
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
///* ��Ǝw������߂� */
///********************/
///////////////////////////////////////////////////////////////
//// ��͐l�S�̂̏d�S����Ń��x�������ő�̂��̂Ƃ���
//// ��̏d�S��艺�̔��F�̈��r�Ƃ���
//// �w��͊�̏d�S����ł��������̂Ƃ���
//// �w��͉E�ƍ������T��
//////////////////////////////////////////////////////////////
//bool image::Check_Skin()
//{
//	int cfx,cfy; //��̏d�S
//	bool FaceFlag=false;
//
//	int dist=0;            //�ex���W�̘A���������F��f��
//	int Avg_dist=0;        //x���W�̘A���������F��f���̕���
//	int Skin_num=0;        //���F���o���ꂽx���W�̐�
//
//	int max_dist = INT_MIN;    //�ő勗��
//	int hand_x1=0,hand_y1=0;   //�w��̍��W(x1,y1)
//	int hand_x2=0,hand_y2=0;   //�w��̍��W(x2,y2)
//	int chand_x1=0,chand_y1=0; //��̏d�S
//	int chand_x2=0,chand_y2=0;
//	bool h_flag1 = false;
//	bool h_flag2 = false;
//	int xx=0,yy=0,g_count=0;
//
//	Person_num=0;   //�摜���̐l�̐�(������)
//
//	Finger=false;
//
//	for(int k=1;k<label;k++){
//		//�O�Ɋ炪���������ꍇ
//		if(pf!=true){
//			if(DetectFace(k)==false){ //OpenCV�Ŋ炪���o����Ȃ��ꍇ
//				//�猟�o(�ȈՔ�)
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
//				//�烉�x�����Ȃ���������h��
//				if(g_count>20){
//					cfx = xx/g_count;
//					cfy = yy/g_count;
//					FaceFlag=true;
//				}
//				xx=0; yy=0; g_count=0; //������
//			}else
//				FaceFlag=true;
//		}
//
//		if(pf==true){ //OpenCV�Ŋ猟�o����Ă���ꍇ
//			//����1�l��������
//			if(fxmax[k]>xmax[k]+50){
//				continue;
//			}
//
//			//�T���̈�𒲐�
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
//#if 0		//�t�@�C���o��
//			ofstream fout;
//			static int nn=1;
//			fout.open("info.txt",ios::app);
//			fout << "N0." << nn << "\n";
//			fout << "Tracking�O" << "\n";
//			fout << "xmin�F" << xmin[k] << "\t" << "xmax�F" << xmax[k] << "\n";
//			fout << "ymin�F" << ymin[k] << "\t" << "ymax�F" << ymax[k] << "\n";
//			//fout << "fxmin�F" << fxmin[k] << "\t" << "fxmax�F" << fxmax[k] << "\n";
//			//fout << "fymin�F" << fymin[k] << "\t" << "fymax�F" << fymax[k] << "\n";
//			fout.close();
//			nn++;
//#endif
//
//			Tracking(k);  //�g���b�L���O
//
//			//����͂�
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
//			//��̈�̐ݒ�
//			for(int j=fymin[k];j<fymax[k];j++){
//				for(int i=fxmin[k];i<fxmax[k];i++){
//					if(isDiff(i,j)==SKIN)
//						isDiff(i,j)=FACE;
//				}
//			}
//			
//			//��̏d�S�����߂�
//			cfx = (fxmin[k]+fxmax[k])/2;
//			cfy = (fymin[k]+fymax[k])/2;
//			FaceFlag=true;
//		}// if(pf==true)
//
//		//�w������߂�
//		if(FaceFlag==true){
//
//			//�T���̈�͂�
//			for(int j=ymin[k];j<ymax[k];j++){
//				B(xmin[k],j)=255; G(xmin[k],j)=0; R(xmin[k],j)=0;
//				B(xmax[k],j)=255; G(xmax[k],j)=0; R(xmax[k],j)=0;
//			}
//			for(int j=xmin[k];j<xmax[k];j++){
//				B(j,ymin[k])=255; G(j,ymin[k])=0; R(j,ymin[k])=0;
//				B(j,ymax[k])=255; G(j,ymax[k])=0; R(j,ymax[k])=0;
//			}
//
//			//���{���̈�C��
//			if(ymin[k]<ycount[k]-50)	ymin[k]=ycount[k]-50;
//
//			//�d�S��艺�̔��F��������Ƃ���
//			for(int j=ymin[k];j<ycount[k];j++){
//				for(int i=xmin[k];i<xmax[k];i++){
//					if(isDiff(i,j)==SKIN)
//						isDiff(i,j)=HAND;
//				}
//			}
//
//			//���x�����O�ɂ��A���T��
//			int value=0;
//			value=labeling_hand(xmin[k],xcount[k],ymin[k],ycount[k]);
//
//			int temp_x=0,temp_y=0;
//
//			if(value>0){
//				//��̈�ň�ԉ������W�����߂�
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
//							//�E���̎�̈�ɐF������
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
//					//if(dist>0 && dist<7 && dist<Avg_dist/4){ //�w���w���Ă���
//					if(dist>0 && dist<Avg_dist/2){
//						h_flag1 =true;
//						Finger=true;
//						dist=0;
//						continue;
//					}
//					Avg_dist = (Avg_dist*Skin_num + dist)/(Skin_num+1); //���ς��C��
//					if(dist>0)
//						Skin_num++;
//					dist=0;  //������
//				}
//				Avg_dist=0; Skin_num=0; //������
//			}else{
//				//���₪�����
//				if(h_label>1){
//					for(int h=1;h<h_label;h++){
//						if(xmax[label+h]<xcount[k] && xmax[label+h]>xmin[k]-20 && ymax[label+h]<=ycount[k]+30){
//							for(int j=ymin[label+h];j<ymax[label+h];j++){
//								for(int i=xmin[label+h];i<xmax[label+h];i++){
//									if(isDiff(i,j)==SKIN){
//										isDiff(i,j)=HAND;
//										//�E���̎�̈�ɐF������
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
//				//��̏d�S�����߂�
//				if(g_count>0){
//					chand_x1=xx/g_count;
//					chand_y1=yy/g_count;
//				}
//
//				//�o��
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
//				//�w��������
//				if(xcount[k]-hand_x1<40 && Finger==false){
//					h_flag1=false;
//				}
//			}
//			delete [] p;
//
//			//������
//			max_dist = INT_MIN;
//			xx=0; yy=0; g_count=0; value=0;
//
//			value=labeling_hand(xcount[k],xmax[k],ymin[k],ycount[k]);
//
//			if(value>0){
//				//��̈�ň�ԉ������W�����߂�
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
//							//�����̎�̈�ɐF������
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
//					Avg_dist = (Avg_dist*Skin_num + dist)/(Skin_num+1); //���ς��C��
//					if(dist>0)
//						Skin_num++;
//					dist=0;  //������
//				}
//			}else{
//				//���₪�����
//				if(h_label>1){
//					for(int h=1;h<h_label;h++){
//						if(xcount[k]<xmin[label+h] && xmin[label+h]<xmax[k]+20 && ymax[label+h]<=ycount[k]+30){
//							for(int j=ymin[label+h];j<ymax[label+h];j++){
//								for(int i=xmin[label+h];i<xmax[label+h];i++){
//									if(isDiff(i,j)==SKIN){
//										isDiff(i,j)=HAND;
//										//�����̎�̈�ɐF������
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
//				//�d�S
//				if(g_count>0){
//					chand_x2=xx/g_count;
//					chand_y2=yy/g_count;
//				}
//				//�o��
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
//				//�w��������
//				if(hand_x2-xcount[k]<40 && Finger==false){
//					h_flag2=false;
//				}
//			}
//			delete [] p;
//
//#if 0		//�t�@�C���o��
//			ofstream fout;
//			fout.open("info.txt",ios::app);
//			fout << "N0." << tmp_frame << "\n";
//			fout << "�w����" << "\n";
//			fout << "�d�Sx�F" << xcount[k] << "\t" << "�d�Sy" << ycount[k] << "\n";
//			fout << "hand_x1�F" << hand_x1 << "\t" << "hand_y1�F" << hand_y1 << "\n";
//			fout << "hand_x2�F" << hand_x2 << "\t" << "hand_y2�F" << hand_y2 << "\n\n";
//			fout.close();
//#endif
//
//			static int count=10;  //�w��������
//
//			//�w�����Ă���ꏊ�����߂�
//			int chand_x,chand_y;
//			int tmp_handx;
//			if(h_flag1==false && h_flag2==false){  //����Ƃ��w���Ă��Ȃ��ꍇ
//				count++;
//				if(count>=10){
//					Finger=false;
//					count=10;
//				}else{
//					Finger=true;
//				}
//				continue;
//			}else if(h_flag1==true && h_flag2==true){  //����Ƃ��w���Ă���ꍇ
//				if(PointDistance(hand_x1,hand_y1,cfx,cfy)>PointDistance(hand_x2,hand_y2,cfx,cfy)){
//					tmp_handx=hand_x1; ymax_point=hand_y1;
//					chand_x=chand_x1;	chand_y=chand_y1;
//					h_flag1=false; h_flag2=false;
//				}else{
//					tmp_handx=hand_x2; ymax_point=hand_y2;
//					chand_x=chand_x2;	chand_y=chand_y2;
//					h_flag1=false; h_flag2=false;
//				}
//			}else if(h_flag1==true){  //�������w�����Ă���ꍇ
//				tmp_handx=hand_x1; ymax_point=hand_y1;
//				chand_x=chand_x1;	chand_y=chand_y1;
//			}else if(h_flag2==true){  //�E�����w�w���Ă���ꍇ
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
//			if(tan>=0){ //����
//				circle_x=tmp_handx-r;
//				xmin_point=circle_x-r;
//				xmax_point=tmp_handx;
//			}else{   //�E��
//				circle_x=tmp_handx+r;
//				xmin_point=tmp_handx;
//				xmax_point=circle_x+r;
//			}
//			//int circle_y = ymax_point-tan*(tmp_handx-circle_x);
//			//ymin_point=circle_y-(ymax_point-circle_y);
//			ymin_point=ymax_point-2*r;
//
//			//����
//			if(xmax_point>Width-3)	xmax_point=Width-3;
//			if(ymin_point<2)		ymin_point=2;
//			if(xmin_point<2)		xmin_point=2;
//			if(ymax_point>Height-3)	ymax_point=Height-3;
//
//			//�����Ă���ꏊ��`��
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
///* OpenCV�ɂ��猟�o */
///**********************/
////bool image::DetectFace(int k)
////{
////	static int count=0;
////
////	static CvMemStorage* storage = 0;             //�I�u�W�F�N�g���o���ʂ�~���Ă���������
////	static CvHaarClassifierCascade* cascade  = 0; //stage classifier�̖؁A���Ȃ킿�Acascade
////	static CvHaarClassifierCascade* cascade1 = 0;
////
////	IplImage *img;         //OpenCV�p�摜�f�[�^�\����
////	
////	if(count==0){
////		//�e���v���[�g�̓ǂݍ���
////		cascade  = (CvHaarClassifierCascade*)cvLoad( "haarcascade_frontalface_alt.xml", 0, 0, 0 );
////		cascade1 = (CvHaarClassifierCascade*)cvLoad( "haarcascade_profileface.xml", 0, 0, 0 );
////
////		storage = cvCreateMemStorage(0);   //�������̈�m��
////		count++;
////	}
////
////	//�摜�T�C�Y�̒���
////	while((xmax[k]-xmin[k])%4!=0)
////		xmin[k]--;
////	if(xmin[k]<0)
////		xmin[k] += 4;
////
////	//�w�b�_�̍쐬�ƃf�[�^�̈�̊m��
////	img = cvCreateImage( cvSize(xmax[k]-xmin[k],ymax[k]-ycount[k]),IPL_DEPTH_8U,3 );
////	img->origin = 1;
////
////	//�摜�f�[�^��img(OpenCV�p)�Ɉڂ�
////	int c=0;
////	for(int j=ycount[k];j<ymax[k];j++){
////		for(int i=xmin[k];i<xmax[k];i++){
////			img->imageData[c++] = B(i,j);
////			img->imageData[c++] = G(i,j);
////			img->imageData[c++] = R(i,j);
////		}
////	}
////
////	//�Ƃ肠�����A�ۑ����ēǂݍ��� �ŏI�I�ɂ̓�������Ŏ����������B
////	cvSaveImage("test.bmp",img);
////	cvReleaseImage( &img );
////
////	bool flag=false;
////
////	IplImage* im = cvLoadImage("test.bmp",1); //�摜�ǂݍ���
////	
////	for(int j=0;j<=1;j++){ //�猟�o(���ʁA���A�㔼�g)
////		int scale = 1;
////		CvPoint pt1, pt2; //x���W��y���W�����\����
////
////		cvClearMemStorage( storage );  //�������H
////
////		//�摜����I�u�W�F�N�g�����o
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
////			//��̔��F�`�F�b�N���s��
////    		CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
////    		pt1.x = r->x*scale;
////    		pt2.x = (r->x+r->width)*scale;
////    		pt1.y = r->y*scale;
////    		pt2.y = (r->y+r->height)*scale;
////			//cvRectangle( im, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
////			
////			//��̈�ɑ΂��āA���F�`�F�b�N
////			int skin=0,total=0;
////			for(int l=ymax[k]-pt2.y;l<ymax[k]-pt1.y;l++){
////				for(int m=xmin[k]+pt1.x;m<xmin[k]+pt2.x;m++){
////					if(isDiff(m,l)==SKIN)
////						skin++;
////					total++;
////				}
////			}
////			//��Ȃ�A���W���i�[
////			if(skin>total/4){
//////				Person_num++;
////				fxmin[k]=xmin[k]+pt1.x; fxmax[k]=xmin[k]+pt2.x;
////				fymin[k]=ymax[k]-pt2.y; fymax[k]=ymax[k]-pt1.y;
////				flag = true;
////				//cvRectangle( im, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
////				break;    //���͐l�̈�Ɋ��� ���v�C��
////			}
////		}//for(���o���ꂽ��̃��[�v)
////		if(j<=1 && flag==true)
////			break;
////	}//for(�猟�o�̃��[�v)
////
////	if(flag==false){
////		cvReleaseImage(&im);
////		pf=false;
////		return false;
////	}
////
////	pf=true;
////	cvReleaseImage( &im ); //�̈���
////	return true;
////}
//
///*********/
///*  �ǐ� */
///*********/
////bool image::Tracking(int k){
////	IplImage* img;
////
////	while((xmax[k]-xmin[k])%4!=0)
////		xmin[k]--;
////	if(xmin[k]<0)
////		xmin[k] += 4;
////
////	//�w�b�_�̍쐬�ƃf�[�^�̈�̊m��
////	img = cvCreateImage( cvSize(xmax[k]-xmin[k],ymax[k]-ycount[k]),IPL_DEPTH_8U,3 );
////	img->origin = 1;
////
////	//�摜�f�[�^��img(OpenCV�p)�Ɉڂ�
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
////	IplImage* frame = cvLoadImage("test.bmp",1); //�摜�ǂݍ���
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
////	//�e�X�g
////	if(track_flag==false){
////		selection.x=fxmin[k];
////		selection.y=Height-fymax[k];
////		selection.width=fxmax[k]-fxmin[k];
////		selection.height=fymax[k]-fymin[k];
////		track_object=-1;
////		track_flag=true;
////	}
////
////	//���߂ĂȂ�A�̈�m��
////	if( !image ){
////		image = cvCreateImage( cvSize(320,240), 8, 3 );
////		cvZero(image);
////		image->origin = frame->origin;
////		hsv = cvCreateImage( cvGetSize(image), 8, 3 );
////		hue = cvCreateImage( cvGetSize(image), 8, 1 );
////		mask = cvCreateImage( cvGetSize(image), 8, 1 );
////		backproject = cvCreateImage( cvGetSize(image), 8, 1 );
////		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );  //�q�X�g�O�����̍쐬
////	}
////
////	//image��frame���R�s�[
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
////    cvCvtColor( image, hsv, CV_BGR2HSV );  //RGB��HSV�ɕϊ�(hsv�Ɋi�[)
////
////	cvInRangeS( hsv, cvScalar(0,smin,vmin,0),
////                cvScalar(180,256,vmax,0), mask ); //2��cvScalar�̊Ԃ̗v�f��������
////    
////    cvSplit( hsv, hue, 0, 0, 0 );   //hsv����Hue���������o��(hue�Ɋi�[)
////
////	if( track_object < 0 ){           //�̈��I����������
////		float max_val = 0.f;
////		cvSetImageROI( hue, selection );     //hue��selection(ROI)���Z�b�g
////		cvSetImageROI( mask, selection );    //mask��selection���Z�b�g
////		cvCalcHist( &hue, hist, 0, mask );   //hist��hue�̃q�X�g�O�������i�[
////		cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );   //hist�̍ő�l���擾
////		cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 ); //�ϊ��H
////		cvResetImageROI( hue );     //hue(ROI)�̃N���A
////		cvResetImageROI( mask );    //mask�̃N���A
////		track_window = selection;   //cvRect�^
////		track_object = 1;           //�g���b�L���O���s��
////	}
////		
////	cvCalcBackProject( &hue, backproject, hist );  //�o�b�N�v���W�F�N�g�̌v�Z
////	cvAnd( backproject, mask, backproject, 0 );    //�_����
////	cvMeanShift(backproject,track_window,
////				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER,10,1),
////				&track_comp);     //�T��
////	track_window = track_comp.rect;
////
////#if 0	//�o��
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
////	cvReleaseImage( &frame ); //�̈���
////
////	return true;
////}
//
///*******************/
///* ���F���o(YCbCr) */
///*******************/
//void image::skincolor()
//{
//	int Yi, Cb, Cr;
//
//	/* RGB����YCbCr�ɕϊ� */
//	//for(int k=1;k<label;k++){
//		for(int j = 0; j < Height; j++){
//			for(int i = 0;i < Width; i++){
//				if(isDiff(i,j)==BACK){
//					Yi = (int)( 0.299  * R(i,j) + 0.587  * G(i,j) + 0.114  * B(i,j));
//					Cb = (int)(-0.1687 * R(i,j) - 0.3312 * G(i,j) + 0.5000 * B(i,j));
//					Cr = (int)( 0.5000 * R(i,j) - 0.4183 * G(i,j) - 0.0816 * B(i,j));
//		
//					//���F�ȊO(else)��΂ɂ���
//					if( ((Yi > 20) && (Yi < 240))
//						&& ((Cb > -20) && (Cb < 20))
//						&& ((Cr > 10) && (Cr < 30)) ){
//							isDiff(i,j)=SKIN;
//					}
//					else{
//						//B(i,j) = 0; G(i,j)=255; R(i,j)=0;
//						//isDiff(i,j)=NON; //BACK�̂܂܂ł����H
//					}
//				}
//			}//for
//		}//for
//	//}
//}
//
///*********************/
///* ���x�����O(4�ߖT) */
///*********************/
//bool image::labeling(int Xmin,int Xmax,int Ymin,int Ymax)
//{
//	int *LUT;         // ���b�N�A�b�v�e�[�u��
//
//	label=1; //�V�K���x��
//
//	/* �̈�m�� */
//	p = new int[Height*Width];
//	LUT = new int[Height*Width];
//
//	LUT[0] = 0;
//	
//	/* ���x���t�� */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* ��f���[�̏ꍇ */
//			if(i==Ymin || i==Ymax-1 || j==Xmin || j==Xmax-1){
//				p[i*Width+j] = 0;
//				continue;
//			}
//			/* ��f�������Ȃ� */
//			if(isDiff(j,i)!=NON){
//				/* ��A���̉�f�����Ƀ��x���t������Ă��Ȃ��Ȃ� */
//				if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = label;
//					LUT[label]=label;
//					label++;    // �V�������x�������
//				}
//				/* ��̉�f���������x���t������Ă���Ȃ� */
//				else if(p[(i-1)*Width+j]!=0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = p[(i-1)*Width+j];
//				}
//				/* ���̉�f���������x���t�����Ă���Ȃ� */
//				else if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]!=0){
//					p[i*Width+j] = p[i*Width+(j-1)];
//				}
//				/* ��A���̉�f�����Ƀ��x���t�����Ă���Ȃ� */
//				else{
//					/* ��̉�f�̕������x�����������ꍇ */
//					if(p[(i-1)*Width+j]<=p[i*Width+(j-1)]){
//						p[i*Width+j] = p[(i-1)*Width+j];
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+(j-1)]] = p[i*Width+j];
//					}
//					/* ���̉�f�̕������x�����������ꍇ */
//					else{
//						p[i*Width+j] = p[i*Width+(j-1)];
//						/* ���x���̓�������ύX */
//						LUT[p[(i-1)*Width+j]] = p[i*Width+j];
//					}
//				}
//			} // if
//			/* ��f��0(��)�̏ꍇ */
//			else{ 
//				p[i*Width+j] = 0;
//			}
//		} // for
//	} // for
//
//
//	/* �t���Ƀ��x���t�� */
//	for(int i=Ymax-2;i>Ymin;i--){
//		for(int j=Xmax-2;j>Xmin;j--){
//			if(p[i*Width+j]!=0){
//				/* ���̉�f�����x���t������Ă���Ȃ� */
//				if(p[(i+1)*Width+j]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[(i+1)*Width+j]]){
//						/* ���x���̓�������ύX */
//						LUT[p[(i+1)*Width+j]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[(i+1)*Width+j]]){
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+j]] = LUT[p[(i+1)*Width+j]];
//					}
//				}
//				/* �E�̉�f�����x���t�����Ă���Ȃ� */
//				if(p[i*Width+(j+1)]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[i*Width+(j+1)]]){
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+(j+1)]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[i*Width+(j+1)]]){
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+j]] = LUT[p[i*Width+(j+1)]];
//					}
//				}
//			}// if
//		}// for
//	}// for
//	
//	/* �e���x���̉�f�����i�[����̈�̊m�� */
//	nlabel = new int[label];
//	//�̂Ƃ͂Ȃꂽ��̕����̔���p
//	skin_num = new int[label];
//
//	/* ������ */
//	for(int i=0;i<label;i++){
//		nlabel[i]   = 0;
//		skin_num[i] = 0;
//	}
//	
//	/* ���x���̕ύX�Ɖ�f���𐔂��� */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* ���x���t������Ă���ꍇ */
//			if(p[i*Width+j] != 0){
//				/* ���x����ύX����K�v������ꍇ�A�ύX���� */
//				while(p[i*Width+j] != LUT[p[i*Width+j]]){
//					p[i*Width+j] = LUT[p[i*Width+j]];
//				}
//				if(isDiff(j,i)==SKIN)	skin_num[p[i*Width+j]]++;
//				nlabel[p[i*Width+j]]++;
//			} // if
//		} // for
//	} // for
//
//	//�̂Ɨ��ꂽ��̕����̔���
//	for(int i=1;i<label;i++){
//		if((nlabel[i]<50) || (skin_num[i]<(nlabel[i]*4/5))){
//			skin_num[i]=0;
//		}
//	}
//
//	/* �̈��� */
//	delete [] LUT;
//
//	//delete [] skin_num;
//	
//	return true;
//}
//
///*********************/
///* ���x�����O(4�ߖT) */
///*********************/
//int image::labeling_hand(int Xmin,int Xmax,int Ymin,int Ymax)
//{
//	int *LUT;         // ���b�N�A�b�v�e�[�u��
//
//	int skin_label=1; //�V�K���x��
//
//	/* �̈�m�� */
//	p = new int[Height*Width];
//	LUT = new int[(Ymax-Ymin)*(Xmax-Xmin)];
//
//	LUT[0] = 0;
//	
//	/* ���x���t�� */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* ��f���[�̏ꍇ */
//			if(i==Ymin || i==Ymax-1 || j==Xmin || j==Xmax-1){
//				p[i*Width+j] = 0;
//				continue;
//			}
//			/* ��f�����F�Ȃ� */
//			if(isDiff(j,i)==HAND){
//				/* ��A���̉�f�����Ƀ��x���t������Ă��Ȃ��Ȃ� */
//				if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = skin_label;
//					LUT[skin_label]=skin_label;
//					skin_label++;    // �V�������x�������
//				}
//				/* ��̉�f���������x���t������Ă���Ȃ� */
//				else if(p[(i-1)*Width+j]!=0 && p[i*Width+(j-1)]==0){
//					p[i*Width+j] = p[(i-1)*Width+j];
//				}
//				/* ���̉�f���������x���t�����Ă���Ȃ� */
//				else if(p[(i-1)*Width+j]==0 && p[i*Width+(j-1)]!=0){
//					p[i*Width+j] = p[i*Width+(j-1)];
//				}
//				/* ��A���̉�f�����Ƀ��x���t�����Ă���Ȃ� */
//				else{
//					/* ��̉�f�̕������x�����������ꍇ */
//					if(p[(i-1)*Width+j]<=p[i*Width+(j-1)]){
//						p[i*Width+j] = p[(i-1)*Width+j];
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+(j-1)]] = p[i*Width+j];
//					}
//					/* ���̉�f�̕������x�����������ꍇ */
//					else{
//						p[i*Width+j] = p[i*Width+(j-1)];
//						/* ���x���̓�������ύX */
//						LUT[p[(i-1)*Width+j]] = p[i*Width+j];
//					}
//				}
//			} // if
//			/* ��f��0(��)�̏ꍇ */
//			else{ 
//				p[i*Width+j] = 0;
//			}
//		} // for
//	} // for
//
//
//	/* �t���Ƀ��x���t�� */
//	for(int i=Ymax-2;i>Ymin;i--){
//		for(int j=Xmax-2;j>Xmin;j--){
//			if(p[i*Width+j]!=0){
//				/* ���̉�f�����x���t������Ă���Ȃ� */
//				if(p[(i+1)*Width+j]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[(i+1)*Width+j]]){
//						/* ���x���̓�������ύX */
//						LUT[p[(i+1)*Width+j]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[(i+1)*Width+j]]){
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+j]] = LUT[p[(i+1)*Width+j]];
//					}
//				}
//				/* �E�̉�f�����x���t�����Ă���Ȃ� */
//				if(p[i*Width+(j+1)]!=0){
//					if(LUT[p[i*Width+j]]<LUT[p[i*Width+(j+1)]]){
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+(j+1)]] = LUT[p[i*Width+j]];
//					}else if(LUT[p[i*Width+j]]>LUT[p[i*Width+(j+1)]]){
//						/* ���x���̓�������ύX */
//						LUT[p[i*Width+j]] = LUT[p[i*Width+(j+1)]];
//					}
//				}
//			}// if
//		}// for
//	}// for
//	
//	/* �e���x���̉�f�����i�[����̈�̊m�� */
//	nlabel = new int[skin_label];
//
//	/* ������ */
//	for(int i=0;i<skin_label;i++){
//		nlabel[i] = 0;
//	}
//	
//	/* ���x���̕ύX�Ɖ�f���𐔂��� */
//	for(int i=Ymin;i<Ymax;i++){
//		for(int j=Xmin;j<Xmax;j++){
//			/* ���x���t������Ă���ꍇ */
//			if(p[i*Width+j] != 0){
//				/* ���x����ύX����K�v������ꍇ�A�ύX���� */
//				while(p[i*Width+j] != LUT[p[i*Width+j]]){
//					p[i*Width+j] = LUT[p[i*Width+j]];
//				}
//				nlabel[p[i*Width+j]]++;
//			} // if
//		} // for
//	} // for
//
//	/* �ő�̈��T�� */
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
//	/* �̈��� */
//	delete [] LUT;
//	delete [] nlabel;
//	
//	return value;
//}
//
////--------------------------------------------------------------
//// name:Get_Area(int xmin[],int ymin[],int xmax[],int ymax[])
//// �w�����Ă���̈��Ԃ�
//// xmin[]:�̈��x���W�̍ŏ��l ymin[]:�̈��y���W�̍ŏ��l
//// xmax[]:�̈��x���W�̍ő�l ymax[]:�̈��y���W�̍ő�l
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
///* 2�_�Ԃ̋��� */
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
///* �I�[�v�j���O */
///****************/
//void image::opening()
//{
//	int *tmp = new int[Width*Height];
//
//	skincolor();
//
//	//������
//	for(int j=0;j<Height;j++){
//		for(int i=0;i<Width;i++){
//			tmp[j*Width+i] = NON;
//		}
//	}
//
//	//���k����
//	for(int j=1;j<Height-2;j++) {
//		for(int i=1;i<Width-2;i++) {
//			if(isDiff(i,j)==SKIN){
//				tmp[j*Width+i]=SKIN;
//			}else if(isDiff(i,j)==BACK){  //�������o�āA���F����Ȃ���f
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
///* �N���[�W���O */
///****************/
//void image::closing()
//{
//	int *tmp = new int[Width*Height];
//
//	for(int n=0;n<4;n++){
//		//������
//		for(int j=0;j<Height;j++){
//			for(int i=0;i<Width;i++){
//				tmp[j*Width+i] = NON;
//			}
//		}
//
//		//�c������
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
///* �f�o�b�N�p���F���o */
///**********************/
//void image::D_skincolor()
//{
//	int Yi, Cb, Cr;
//
//	/* RGB����YCbCr�ɕϊ� */
//	for(int j = 0; j < Height; j++){
//		for(int i = 0;i < Width; i++){
//			//if(isDiff(i,j)==BACK){
//				Yi = (int)( 0.299  * R(i,j) + 0.587  * G(i,j) + 0.114  * B(i,j));
//				Cb = (int)(-0.1687 * R(i,j) - 0.3312 * G(i,j) + 0.5000 * B(i,j));
//				Cr = (int)( 0.5000 * R(i,j) - 0.4183 * G(i,j) - 0.0816 * B(i,j));
//
//#if 0		//�t�@�C���o��
//			ofstream fout;
//			fout.open("info.txt",ios::app);
//			fout << "x:" << i << "\t" << "y:" << j << "\n";
//			fout << "Yi�F" << Yi << "\t" << "Cb�F" << Cb << "\t" << "Cr:" << Cr << "\n";
//			fout.close();
//#endif
//	
//				//���F�ȊO(else)��΂ɂ���
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
//					//isDiff(i,j)=NON; //BACK�̂܂܂ł����H
//				}
//			//}
//		}//for
//	}//for
//}
//
///*********/
///* 2�l�� */
///*********/
////bool image::blackWhite()
////{
////	unsigned char level;    // ��f�̔Z�x�l
////	unsigned int hist[256]; // �e�Z�x�l�̉�f��
////	int threshold=0;        // 臒l
////	int n1,n2;              // 臒l�����A�ȏ�̃N���X�̉�f��
////	unsigned long tmp;      // 臒l�����A�ȏ�̃N���X�̑S�Ẳ�f�̔Z�x�̘a
////	int sum=0,count=0;      // �S��f�̔Z�x�̘a�A�S��f��
////	double ave;             // �S��f�̕��ϔZ�x�l
////	double ave1, ave2;      // 臒l�����A�ȏ�̃N���X�̕��ϔZ�x�l
////	double var1,var2;       // 臒l�����A�ȏ�̃N���X�̕��U�̕��q
////	double var_w;           // �N���X�����U
////	double var_b;           // �N���X�ԕ��U
////	double r;               // �N���X�����U�ƃN���X�ԕ��U�̔�
////	double max=-1.0;        // ���ʔ�̍ő�l
////	int i,j,t;
////	
////	/* �q�X�g�O�����̏����� */
////	for(i=0;i<256;i++) {
////		hist[i] = 0;
////	}
////
////	/* �q�X�g�O�����̍쐬 */
////	for(i=0;i<Height;i++) {
////		for(j=0;j<Width;j++) {
////			level = B(j,i);
////			hist[level] ++;
////			sum += B(j,i);
////			count++;
////		}
////	}
////	
////	/* �S��f�̕��ϔZ�x�l�����߂� */
////	ave = sum/count;
////
////	/* 臒l��0�`255�ɕς��Ȃ���A�œK�l�����肷�� */
////	for(t=0;t<=255;t++){
////		/* ������ */
////		n1 = n2 = 0;
////		ave1 = ave2 = 0;
////		var1 = var2 = 0;
////		tmp= 0;
////		
////		/* 臒l�����̃N���X */
////		for( i = 0; i < t; i++ ) {
////			n1  += hist[i];
////			tmp += hist[i]*i;
////		}
////		/* ���ϔZ�x�l�����߂� */
////		if(n1!=0){
////			ave1 = (double)tmp / (double)n1;
////		}
////
////		/* ���U�̕��q�����߂� */
////		for(i=0;i<t;i++) {
////			var1 = var1 + (i-ave1)*(i-ave1)*hist[i];
////		}
////		/* ���U�����߂� */
////		if ( n1 != 0 ) {
////			var1 = var1/(double)n1;
////		}
////
////		tmp = 0; // ������
////		
////		/* 臒l�ȏ�̃N���X */
////		for(i=t;i<=255;i++){
////			n2 = n2 + hist[i];
////			tmp = tmp + hist[i]*i;
////		}
////		/* ���ϔZ�x�l�����߂� */
////		if(n2!=0){ 
////			ave2 = (double)tmp / (double)n2;
////		}
////		/* ���U�̕��q�����߂� */
////		for(i=t;i<255;i++) {
////            var2 = var2 + (i-ave2)*(i-ave2)*hist[i];
////		}
////		/* ���U�����߂� */
////		if ( n2 != 0 ) { 
////			var2 = var2/(double)n2;
////		}
////
////		/* �N���X�����U�����߂� */
////		var_w = (n1*var1 + n2*var2);	// ����(n1+n2)�͕K�v�Ȃ�����ȗ�
////		/* �N���X�ԕ��U�����߂� */
////		var_b = n1*(ave1-ave)*(ave1-ave) + n2*(ave2-ave)*(ave2-ave);
////		
////		r  = var_b / var_w;
////		if (r>max){
////			max = r;
////			threshold = t;
////		}
////	}
////
////	cout << "2�l����臒l�F" << threshold << '\n';
////	
////	/* 臒l�����Ȃ�0�A臒l�ȏ�Ȃ�255 */
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
///* �G�b�W���o */
///**************/
////bool image::edgedetection()
////{
////	int *x,*y;
////	int min,max;
////	double p;
////	int i,j;
////	
////	/* �̈�m�� */
////	x = new int[Height*Width];
////	y = new int[Height*Width];
////	
////	/* �t�B���^�����O */
////	for(i=0;i<Height;i++){
////		for(j=0;j<Width;j++){
////			/* ��f���[�Ȃ� */
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
////	/* ������ */
////	min = B(0,0);
////	max = B(0,0);
////	
////	/* �ŏ��l�A�ő�l�����߂� */
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
////	/* �X�P�[���ϊ� */
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
////	/* �̈��� */
////	delete [] x;
////	delete [] y;
////	
////	return true;
////}