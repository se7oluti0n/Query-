#pragma once

//�����̉�f�̑���
enum DiffType{NON,BACK,NEITHER,SKIN,HAIR,FACE,HAND};

class image {
private:
	
	int Width;              //�摜�̕�
	int Height;             //�摜�̍���
	unsigned char *pixel;   //�摜�f�[�^(RGB)
	unsigned char *pixel_Y; //�P�x�l
	double *pixels_Y_d;

	int *p;           //�e��f�̃��x���ԍ�
	int *nlabel;	  //�e���x���̉�f��
	int *xcount;      //�e���x���̏d�S�v�Z�p
	int *ycount;      //�e���x���̏d�S�v�Z�p
	int label;        //���o���ꂽ���̂̃��x����+1(�Y�����ƃ��x���ԍ��͓���)

	int *xmin;        //�l(��)�̊O�ڒ����Y��x���W�̍ŏ��l
	int *ymin;        //�l(��)�̊O�ڒ����Y��y���W�̍ŏ��l
	int *xmax;        //�l(��)�̊O�ڒ����Y��x���W�̍ő�l
	int *ymax;        //�l(��)�̊O�ڒ����Y��y���W�̍ő�l

	int h_label;      //�̂Ɨ��ꂽ��̃��x����
	
	int fxmin[5];     //�l�̊�̊O�ڒ����`�̂����W�̍ŏ��l
	int fymin[5];     //�l�̊�̊O�ڒ����`�̂����W�̍ŏ��l
	int fxmax[5];     //�l�̊�̊O�ڒ����`�̂����W�̍ő�l
	int fymax[5];     //�l�̊�̊O�ڒ����`�̂����W�̍ő�l
	int Person_num;   //�摜���̐l�̐�

	bool voice;       //�������͂����邩�ǂ���
	bool Finger;      //�w�����Ă��邩�ǂ���
	bool pf;          //�l�t���O

	int *skin_num;    //���x���t�����ꂽ�̈�̔��F�̐�

	bool track_flag;

	int xmin_point;   //�w�����̈��x���W�̍ŏ��l
	int ymin_point;   //�w�����̈��y���W�̍ŏ��l
	int xmax_point;   //�w�����̈��x���W�̍ő�l
	int ymax_point;   //�w�����̈��y���W�̍ő�l
	
public:
	
	image(){
		Width  = 320;
		Height = 240;
		init();
	}
	image(const int w, const int h){
		Width  = w;
		Height = h;
		init();
	}
	~image(){ /*release();*/ }
	
	/* �摜�f�[�^�T�C�Y�̊m�� */
	void init(){
		pixel      = new unsigned char[Width*Height*3];
		pixel_Y    = new unsigned char[Width*Height*3];
		pixels_Y_d = new double[Width*Height*3];
	}

	/* �摜�f�[�^�T�C�Y�̉�� */
	void pix_reset(void){ pixel=0; }
	void release(void){ 
		if(pixel)      delete [] pixel; 
		if(pixel_Y)    delete [] pixel_Y;
		if(pixels_Y_d) delete [] pixels_Y_d;
	}
	
	/* RGB */
	unsigned char &R(int x,int y){ return pixel[(y*Width+x)*3+2]; }
	unsigned char &G(int x,int y){ return pixel[(y*Width+x)*3+1]; }
	unsigned char &B(int x,int y){ return pixel[(y*Width+x)*3]; }

	/* �P�x�l */
	unsigned char &Y(int x,int y){ return pixel_Y[(y*Width+x)*3+2]; }

	//�����̑���
	unsigned char &isDiff(const int x,const int y){ return pixel_Y[(y*Width+x)*3]; }

	//�덷
	double &E(const int x,const int y  ){ return pixels_Y_d[(y*Width+x)*3  ];}
	double &Y_d(const int x,const int y){ return pixels_Y_d[(y*Width+x)*3+2];}

	/* �Ď� */
	int watch(int frame);

	/* ���̌��m(�ω�������������) */
	void BackDiff(image &back,image &input);  //�w�i����
	void opening();         //�I�[�v�j���O
	void closing();         //�N���[�W���O

	/* �l���o */
	bool Check_User();            //�l�̌��o���s��
	bool Check_Skin();            //��A�w������߂�
	bool labeling(int Xmin,int Xmax,int Ymin,int Ymax);  //���x�����O
	void skincolor();          //���F���o(YCbCr)
	bool DetectFace(int k);    //OpenCV�ɂ���(or�㔼�g)���o
	bool Tracking(int k);      //�ǐ�

	int labeling_hand(int Xmin,int Xmax,int Ymin,int Ymax); //���F�̃��x�����O

	void M_estimator(image &pre,image &now); //�l����
	int PointDistance(const int a,const int b,const int c,const int d);

	/* �����ۑ� */
	//bool blackWhite();    //2�l��
	//bool edgedetection(); //�G�b�W���o

	bool Get_Area(int xmin[],int ymin[],int xmax[],int ymax[]);   //�w�����̈��Ԃ�
	void set_voice(){voice = true;};

	//�f�o�b�N�p
	void D_skincolor();  //���F���o
};
