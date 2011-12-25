#include "stdafx.h"
#include <iostream>
using namespace std;
#include "SeekUndetection.h"
#include "SceneState.h"

int CseekUndetection::Reference(int st,int ed)
{
	static Scene state;
	IplImage *input=0,*human=0;
	for(int frame=st;frame<ed;frame++){
		//ƒtƒ@ƒCƒ‹“Ç‚Ýž‚Ý
		char file[50];
		char tmp[10];

		//”wŒi‰æ‘œ
		back=0;

		sprintf_s(file,"./Human-DB/Original/");
		sprintf_s(tmp,"%lu",st);
		strcat_s(file,tmp);
		strcat_s(file,"/");
		sprintf_s(tmp,"%08lu",frame);
		strcat_s(file,tmp);
		strcat_s(file,".bmp");
		input = cvLoadImage(file,CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

		//‰æ‘œ‚ª‚È‚¢
		if(input==0){
			cout << file << ":No Image\n";
			continue;
		}

		sprintf_s(file,"./Human-DB/Process/");
		sprintf_s(tmp,"%lu",st);
		strcat_s(file,tmp);
		strcat_s(file,"/");
		sprintf_s(tmp,"%08lu",frame);
		strcat_s(file,tmp);
		strcat_s(file,".bmp");
		human = cvLoadImage(file,CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

		if(frame==st){
			sprintf_s(file,"./Human-DB/Original/");
			sprintf_s(tmp,"%lu",st);
			strcat_s(file,tmp);
			strcat_s(file,"/");
			sprintf_s(tmp,"%08lu",st);
			strcat_s(file,tmp);
			strcat_s(file,".bmp");
			back = cvLoadImage(file,CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
			bgray=cvCreateImage(cvSize(back->width,back->height),IPL_DEPTH_8U,1);
			cvCvtColor(back, bgray, CV_BGR2GRAY);
			state.SetArea(xmin,xmax,240-ymax,240-ymin);
		}

		BackDiff(input,human);

		bool flag=state.check(input);

		cvNamedWindow("Seek-UndetectionScene",1);
		cvMoveWindow("Seek-UndetectionScene",1200,500);
		cvShowImage("Seek-UndetectionScene",input);
		//cvWaitKey(10);

		cvReleaseImage(&input);
		cvReleaseImage(&human);

		if(flag==true){
			cout << frame << "\n";
			cvReleaseImage(&back);
			cvReleaseImage(&bgray);
			cvDestroyWindow("Seek-UndetectionScene");
			return frame;
		}
	}

	cvReleaseImage(&back);
	cvReleaseImage(&bgray);
	cvDestroyWindow("Seek-UndetectionScene");

	return 0;
}

void CseekUndetection::BackDiff(IplImage *input,IplImage *human)
{
	double diff;
	const int b_threshold = 15;

	IplImage *gray = cvCreateImage(cvSize(input->width,input->height),IPL_DEPTH_8U,1);
	cvCvtColor(input, gray, CV_BGR2GRAY);

	int tymin = 240-ymax;
	int tymax = 240-ymin;
	for(int j=tymin; j<tymax; j++){
		for(int i=xmin; i<xmax; i++){
			uchar tp = human->imageData[(human->widthStep*j+i*3)+1];
			if(tp!=255){
				diff = sqrt((double)((bgray->imageData[bgray->widthStep*j+i] - gray->imageData[gray->widthStep*j+i])
					* (bgray->imageData[bgray->widthStep*j+i] - gray->imageData[gray->widthStep*j+i])));
				if(diff < b_threshold){//·‚ªè‡’lˆÈ‰º‚È‚ç”wŒi
					input->imageData[(input->widthStep*j+i*3)] = 0;
					input->imageData[(input->widthStep*j+i*3)+1] = 0;
					input->imageData[(input->widthStep*j+i*3)+2] = 0;
				}else{
					input->imageData[(input->widthStep*j+i*3)] = 255;
					input->imageData[(input->widthStep*j+i*3)+1] = 255;
					input->imageData[(input->widthStep*j+i*3)+2] = 255;
				}
			}else{
				input->imageData[(input->widthStep*j+i*3)] = 0;
				input->imageData[(input->widthStep*j+i*3)+1] = 0;
				input->imageData[(input->widthStep*j+i*3)+2] = 0;
			}
		}
	}
	cvReleaseImage(&gray);
}

void CseekUndetection::Init()
{
	xmin=xmax=ymin=ymax=0;
	back=0;
}