#include "stdafx.h"
#include "DispUndetection.h"
#include "cv.h"
#include "highgui.h"

void CdispUndetection::DispScene(int folder, int frame)
{
	IplImage *input;

	for(int i=frame-20;i<frame+20;i++){
		char file[50];
		char tmp[10];

		sprintf_s(file,"./Human-DB/Original/");
		sprintf_s(tmp,"%lu",folder);
		strcat_s(file,tmp);
		strcat_s(file,"/");
		sprintf_s(tmp,"%08lu",i);
		strcat_s(file,tmp);
		strcat_s(file,".bmp");
		input = cvLoadImage(file,CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

		if(input==0)	continue;

		cvNamedWindow("Scene",1);
		cvMoveWindow("Scene",200,200);
		cvShowImage("Scene",input);
		cvWaitKey(100);
	}
	cvDestroyWindow("Scene");
	cvReleaseImage(&input);
}