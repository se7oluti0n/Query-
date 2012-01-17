// QueryByMouseDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "QueryByMouse.h"
#include "QueryByMouseDlg.h"
#include "QueryHuman.h"
#include "SeekUndetection.h"
#include "DispUndetection.h"
#include "FileOperator.h"
#include "cv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CCamera camera;   //�J��������N���X
CCamera *cp;      //�J��������N���X(�X���b�h�p)
int CameraNumber;  //�g�p�J�����ԍ�

Multi_Disp SceneOut;  //��F�����̃V�[���o�͗p�_�C�A���O
Multi_Disp *so;
//�ĉ��ߗp?+++++++++++++++++++++++++++++++++++++
int lnum,leftnum,mnum,total_obj_num;
//int LData[10],LtData[10],MData[10];
Object *to,*LData1,*LtData1,*MData1;
int tflag;
//++++++++++++++++++++++++++++++++++++++++++++++
CqueryHuman *hp,query;
int SceneNum = 0;
CseekUndetection *sp,s;
int CandFrame[10]={0};
int CandFolder[10]={0};
int CandNum=0;
int Qtime=-1;       //�����p�̎���
CdispUndetection dp;

// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQueryByMouseDlg �_�C�A���O





CQueryByMouseDlg::CQueryByMouseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQueryByMouseDlg::IDD, pParent)
	, m_xvTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQueryByMouseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO1, m_xvTime);
	DDX_Control(pDX, IDC_COMBO1, m_xcTime);
}

BEGIN_MESSAGE_MAP(CQueryByMouseDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_OFFLINE, &CQueryByMouseDlg::OnBnClickedOffline)
	ON_BN_CLICKED(IDC_PUT, &CQueryByMouseDlg::OnBnClickedPut)
	ON_BN_CLICKED(IDC_YES, &CQueryByMouseDlg::OnBnClickedYes)
	ON_BN_CLICKED(IDC_NO, &CQueryByMouseDlg::OnBnClickedNo)
	ON_BN_CLICKED(IDC_THANKS, &CQueryByMouseDlg::OnBnClickedThanks)
	ON_BN_CLICKED(IDC_LEAVE, &CQueryByMouseDlg::OnBnClickedLeave)
	ON_BN_CLICKED(IDC_ONLINE, &CQueryByMouseDlg::OnBnClickedOnline)
	ON_BN_CLICKED(IDC_TIME, &CQueryByMouseDlg::OnBnClickedTime)
	ON_BN_CLICKED(IDC_NUM, &CQueryByMouseDlg::OnBnClickedNum)
END_MESSAGE_MAP()

//�X���b�h(�I�����C��)
static UINT AFX_CDECL ThreadProcCalc(LPVOID pParam)
{
	cp=(CCamera*)pParam;
	cp->SendCommand(CameraNumber,5);
	return 0;
}

//�X���b�h(�I�t���C��)
static UINT AFX_CDECL ThreadOffLine(LPVOID pParam)
{
	cp=(CCamera*)pParam;
	cp->imageKinect();
	//cp->OffLine();
	return 0;
}

//����ߌ��V�[���\���p�X���b�h
static UINT AFX_CDECL ThreadMDisp(LPVOID pParam)
{
	so=(Multi_Disp*)pParam;
	if(tflag==4) //���u��
		so->indicate(MData1,mnum);
	else if(tflag==7) //������
		so->indicate(LData1,lnum);

	return 0;
}

//�����o���V�[���\���p�X���b�h
static UINT AFX_CDECL ThreadMDispUndetection(LPVOID pParam)
{
	int *DispFrame;
	int *DispFolder;
	int DispNum=CandNum;
	DispFrame = new int[DispNum];
	DispFolder = new int[DispNum];
	for(int i=0;i<DispNum;i++){
		DispFolder[i] = CandFolder[i];
		DispFrame[i]  = CandFrame[i];
	}

	so=(Multi_Disp*)pParam;
	so->indicate_Undetection(DispFolder,DispFrame,DispNum);
	
	delete [] DispFolder;
	delete [] DispFrame;

	return 0;
}

//�X���b�h(�����o�V�[��������--�ŐV����ߋ��Ɍ�������)
static UINT AFX_CDECL ThreadSeek(LPVOID pParam)
{
	sp=(CseekUndetection*)pParam;
	for(int i=0;i<SceneNum;i++){
		if(query.Get_hour(i)>=Qtime){
			bool sflag=true;
			CandFrame[CandNum]=s.Reference(query.Get_sframe(i),query.Get_eframe(i));
			if(CandFrame[CandNum]!=0){
				//Event-DB���̃V�[���Ɠ������̂͌��ɂ��Ȃ�
				for(int j=0;j<lnum;j++){
					if((LData1[j].frame-15 < CandFrame[CandNum]) && (CandFrame[CandNum] < LData1[j].frame+15))
						sflag=false;
				}
				for(int j=0;j<mnum;j++){
					if((MData1[j].frame-15 < CandFrame[CandNum]) && (CandFrame[CandNum] < MData1[j].frame+15))
						sflag=false;
				}
				for(int j=0;j<leftnum;j++){
					if((LtData1[j].frame-15 < CandFrame[CandNum]) && (CandFrame[CandNum] < LtData1[j].frame+15))
						sflag=false;
				}
				if(sflag==true){
					CandFolder[CandNum]=query.Get_sframe(i);
					//�D�揇�ʓ���ւ�����ׂ����낤���ǁA���͂���
					CandNum++;
				}
			}
		}
		else{
			break;
		}
	}
	return 0;
}

//�X���b�h(HumanDB����)
static UINT AFX_CDECL ThreadRefHuman(LPVOID pParam)
{
	hp=(CqueryHuman*)pParam;
	SceneNum = hp->Reference();
	if(SceneNum>0)
		AfxBeginThread(ThreadSeek,&s,THREAD_PRIORITY_BELOW_NORMAL);
	return 0;
}


// CQueryByMouseDlg ���b�Z�[�W �n���h��

BOOL CQueryByMouseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	Init();
	CameraNumber=0;  //�g���Ă���J�����̔ԍ�(������)
	dummy.Create(IDD_OUTPUT,NULL);
	SceneOut.Create(IDD_MULTI_DISP,NULL);

	SceneOut.ShowWindow(SW_SHOW);

	//���X�g
	m_xcTime.InsertString(-1,"1");	m_xcTime.InsertString(-1,"2");	m_xcTime.InsertString(-1,"3");
	m_xcTime.InsertString(-1,"4");	m_xcTime.InsertString(-1,"5");	m_xcTime.InsertString(-1,"6");
	m_xcTime.InsertString(-1,"7");	m_xcTime.InsertString(-1,"8");	m_xcTime.InsertString(-1,"9");
	m_xcTime.InsertString(-1,"10");	m_xcTime.InsertString(-1,"11");	m_xcTime.InsertString(-1,"12");

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CQueryByMouseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CQueryByMouseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CQueryByMouseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//---------------------------------------------------------
// Name:DispSystemmsg()
//�u�V�X�e���̔��b�v�Ƀ��b�Z�[�W��\������֐�
//
//Input:string�^�̕�����
//Outout:�_�C�A���O�̉�ʂɂ���ID�̏ꏊ�ɕ�����\��
//---------------------------------------------------------
void CQueryByMouseDlg::DispSystemmsg(const std::string &msg)
{
	SetDlgItemText(IDC_EDIT_SYSTEM, "");   //���[�U���b
	((CEdit*)GetDlgItem(IDC_EDIT_SYSTEM))->ReplaceSel(msg.c_str());
}

//---------------------------------------------------------
// NAME:DispUsermsg()
//�u���[�U�̔��b�v�Ƀ��b�Z�[�W��\������֐�
//
//Input:string�^�̕�����
//Outout:�_�C�A���O�̉�ʂɂ���ID�̏ꏊ�ɕ�����\��
//---------------------------------------------------------
void CQueryByMouseDlg::DispUsermsg(const std::string &msg)
{
	SetDlgItemText(IDC_EDIT_USER, "");   //���[�U���b
	((CEdit*)GetDlgItem(IDC_EDIT_USER))->ReplaceSel(msg.c_str());
}

//------------------------------------------------------------
// Name:ControlDialog() [�쐬�ҁF�ЎR�@2004�N����]
//                      [���ǎҁF�q�@]
//  
// �Θb���䕔�F�����ŔF�����ʂ�p���Đ��䂵�Ă���
// Input:�Ȃ�
//Output:�Ȃ�
//------------------------------------------------------------
void CQueryByMouseDlg::ControlDialog()
{
	//###########################################################
	//�������牺�őΘb���� �u�Θb����̎菇�v
	// 1.�F�����ʂ�F���\���̂ɕ����ē����
	// 2.�^�X�N�𐶐�
	// 3.�X���b�g�I�u�W�F�N�g�ɑ��
	//   3-1.�^�X�N�̃X���b�g�\���ɓ���邽�߂ɔF�����ʂ̕⊮
	//   3-2.�^�X�N�̃X���b�g�\���ɓ���邽�߂ɔF�����ʂ̐���
	//   3-3.���[�U�̔��b�t���O�𗧂Ă�
	// 4.�V�X�e�����b����
	//   4-1.�X���b�g�̏�ԁ{���[�U�̔��b�t���O���甭�b������
	//   4-2.�V�X�e�����b�t���O�𗧂Ă�
	//###########################################################

	// 1.�܂��F�����ʂ�F�����ʍ\���̂Ɋi�[����
	SplitResult();//�F�����ʂ�F���\���̂ɕ����ē����
	//WriteResultLogFile();//�t�@�C���ɔF�����ʂ���������

	// 2.�L�[���[�h������s����^�X�N�����߂�
	//����̓��[�U�̑�P���b(record��1)�̎��̂ݍs��
	if(record==1){

		//�Ƃ肠�����ASetTask()���R�����g�A�E�g(4/15)
		SetTask();   //�^�X�N�̌���


		if(taskflag>=1){ //�^�X�N�������
			//DispSystemmsg("---[�₢���킹�̎�t]---\r\n");
		}else{ //�^�X�N�������o
			//DispSystemmsg("---[�₢���킹�ȊO�𐶐��ł��܂���]---\r\n");
		}
	}

	//3.�X���b�g�I�u�W�F�N�g�ɑ��
	IntoSlot(Qslot);  //�X���b�g�X�V


	//4.�V�X�e�����b���� = �X���b�g�̏��+���[�U�̔��b�t���O
	system.InitContents();  //���b���e�t���O�̏�����
	GenerateSystemMsg(Qslot);//���b�������[�`��
}

//--------------------------------------------------------------------------
//NAME:GessTask()
//[�^�X�N�̐���]
//Rule1:�u���v��u�N�v��������΁A�₢���킹
//Rule2: �����Ŕ��f�F�u�u�����v�A�u�����ė����v�A�u����v�͕��u��
//                   �u�����čs�����v�A�u�����Ȃ����v�͕�����
//
// Input:�F�����ʍ\���� st �Aint�^ N-best
// Output:int�^�̃t���Ogesstask
//--------------------------------------------------------------------------
int CQueryByMouseDlg::GessTask()
{
	int gesstask = 0;//�^�X�N����t���O�F0�͖����o�A1�͖₢���킹(�l)�A2�͖₢���킹(����)�A3�͖₢���킹(�l�A����)
					 //					 4�͕��u��(�l)�A5�͕��u��(����)�A6�͕��u��(�l�A����)
	                 //                  7�͕�����(�l)�A8�͕�����(����)�A9�͕�����(�l�A����)
	bool hito=false;
	bool time=false;

	for(nbest = 0 ; nbest < MAX_CAND_NUM ; nbest++){//��␔�����J��Ԃ�
		for(int i = 0; i < st[nbest].ElementsNum ;i++){
			if(st[nbest].CategoryID[i]==5){  //�u���v���������
				time = true;
			}else if(st[nbest].CategoryID[i]==6){ //�u�N�v���������
				hito = true;
			}else if(st[nbest].CategoryID[i]==7){ //���u���̖₢���킹
				gesstask = 4;
			}else if(st[nbest].CategoryID[i]==8){ //�������̖₢���킹
				gesstask = 7;
			}
		}
	}

	//�^�X�N�̌���
	if(hito==true)	gesstask+=1;
	if(time==true)	gesstask+=2;

	if(gesstask == 0) {		
		record = 0;//�L�[���[�h�����o�����܂őΘb�͐i�܂Ȃ��悤�ɂ���
		return 0;
	}
	
	return gesstask;
}

//------------------------------------------------------------------------
//NAME:IntoSlot()
//�₢���킹�X���b�g�𖄂߂Ă���
//
//
//-----------------------------------------------------------------------
int CQueryByMouseDlg::IntoSlot(CQuerySlot &Q)
{
	bool AreaFlag=false;

	//if(pointx)	AreaFlag=true;

	nbest = 0;//�����̔F�����ʂ݂̂���������0�ɂ��Ă���

	if(taskflag==0)	return 0;

	//���ӂ̌��t�Ȃ�I��(������) 
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==13){
			endflag=true;
		}
	}

	//����
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==19){
			Q.SetTime(st[nbest].WordID[i]+12);
			//Q.SetTime(12);
			return 0;
		}
	}

	//�ԍ�
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==100){
			so->Stop();
			Sleep(200);
			//����߃��[�h
			if(Q.GetCorNum()==1){
				if(taskflag==4)
					Q.SetObjectNum(MData1[st[nbest].WordID[i]-1].frame);
				else if(taskflag==7)
					Q.SetObjectNum(LData1[st[nbest].WordID[i]-1].frame);
			} 
			//�����o���[�h
			else if(Q.GetCorNum()==2){
				Q.SetObjectNum(st[nbest].WordID[i]-1);
			}
		}
	}

	//�ԓ����m��̏ꍇ
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==9){
			//
			user=system;
			Q = PreSlot;

			if(system.GetObject()){  //�V�X�e�������̂ɂ��Ċm�F�����Ă����ꍇ
				Q.SetObjectOK(true);     //����OK
			}
			if(system.GetArea()){ //�V�X�e�����ꏊ�ɂ��Ċm�F�����Ă����ꍇ
				Q.SetAreaOK(true);    //�ꏊOK
			}
			if(system.GetResult()){ //�V�X�e�������ʂ���������
				endflag=true;
			}
			if(system.GetTime()){   //���Ԋm�F
				Q.SetTimeOK(true);
				if(Q.GetCorNum()!=1){ //���ʂ̌����Ȃ玞�Ԃōi��
					int num=Database.NarrowByTime(Q.GetTimeNum());
					Q.SetCandObjNum(num);
					Q.SetObjectNum(Database.Give_Value());
				}
			}
		}
	}

	//�ԓ����ے�̏ꍇ
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==10){
			if(system.GetObject()){  //�V�X�e�������̂ɂ��Ċm�F�����Ă����ꍇ
				//���̌��̍X�V
				Database.Renewal(); //�����̍X�V
				Q.SetCandObjNum(Q.GetCandObjNum()-1); //��␔�i�[
				if(Q.GetCandObjNum()>0)
					Q.SetObjectNum(Database.Give_Value()); //�ł��D��x���������̂̔ԍ�
			}
			if(system.GetArea()){  //�V�X�e�����ꏊ�ɂ��Ċm�F�����Ă����ꍇ
				Q.SetArea_xmax(0); Q.SetArea_xmin(0);
				Q.SetArea_ymax(0); Q.SetArea_ymin(0);
			}
			if(system.GetScene()){ //�V�X�e��������߁E�����o���V�[����񎦂����ꍇ
				so->Stop();
				Sleep(200);
				Q.SetObjectNum(-1);
				Q.SetCorNum(Q.GetCorNum()+1);
			}
		}
	}

	//++++++++++++++++++++++++++++++++++++++++++
	// ���[�U���w�����Ă���̈�𒲂ׂ�
	// 1.�ꏊ���܂��m�肵�Ă��Ȃ��ꍇ
	// 2.�V�X�e�����w������v�������ꍇ
	// 3.���[�U���ꏊ�̒����������ꍇ
	//
	//++++++++++++++++++++++++++++++++++++++++++
	int count = 0;
	if(Q.GetArea()==0 && system.GetArea()!=true){ //�ꏊ���܂��m�肵�Ă��Ȃ�
		while(count < 10){
			AreaFlag=ReferenceObject();
			if (AreaFlag) count++;
			Sleep(10);
		}
		if(count >= 10){ //�ꏊ�����������Ƃ��A�X���b�g�X�V
			Q.SetArea_xmin(xmin[0]); Q.SetArea_xmax(xmax[0]);
			Q.SetArea_ymin(ymin[0]); Q.SetArea_ymax(ymax[0]);
		//	Q.SetArea_xmin(pointx-15); Q.SetArea_xmax(pointx+15);
		//	Q.SetArea_ymin(pointy-15); Q.SetArea_ymax(pointy+15);
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++
	// �������Č���T��
	//
	//+++++++++++++++++++++++++++++++++++++++++
	if(Q.GetArea()!=0 && Q.GetCandObjNum()==0 && system.GetObject()!=true && system.GetArea()!=true){ //���߂Ă̌���
		int num=Database.Reference(xmin[0],ymin[0],xmax[0],ymax[0],taskflag);
		//int num=Database.Reference(pointx-10,pointy-10,pointx+10,pointy+10,taskflag);
		if(num>0){ //��₪�����
			Q.SetCandObjNum(num); //��␔�i�[
			Q.SetObjectNum(Database.Give_Value()); //�ł��D��x���������̂̔ԍ�
		}
	}

	PreSlot = Q;  //�P���b�O�̃X���b�g�̏���ۑ����Ă����D���[���ɂ���Ă͏�������邩������Ȃ�����

	return 1;
}


//--------------------------------------------------------------
// name:ReferenceObject()
// ���[�U�̎w�����̈�𓾂�
//
//Input:
//Output:�Ȃ�
//---------------------------------------------------------------
bool CQueryByMouseDlg::ReferenceObject()
{
	if(cp->get_finger(xmin,ymin,xmax,ymax)==true){
		return true;
	}else{
		return false;
	}
}


//-------------------------------------
// name:SplitResult()
//�F�����ʂ�F���\���̂ɕ����ē����
//
// Input:�Ȃ�
//Output:int�^
//-------------------------------------
int CQueryByMouseDlg::SplitResult()
{
	char WordText[MAX_CHAR_NUM_DIC];//�i��
	char CategoryID[MAX_CATID_NAME];//�J�e�S��ID
	char WordID[MAX_WORDID_NAME];//���ID

	st[0].CategoryID=(int*)calloc(MAX_ELEMENT_NUM,sizeof(int));
	st[0].WordID=(int*)calloc(MAX_ELEMENT_NUM,sizeof(int));
	st[0].ElementConf=(float*)calloc(MAX_ELEMENT_NUM,sizeof(float));
	st[0].ElementText=(char**)balloc(MAX_ELEMENT_NUM,MAX_CHAR_NUM_DIC,sizeof(char));

	for(nbest = 0 ; nbest < MAX_CAND_NUM ; nbest++){//��␔�����J��Ԃ�
		int k = 0;
		for(int j=0; j<st[nbest].ElementsNum; j++){
			if(SplitWord(Word[j], '#', '$', WordText, CategoryID , WordID)){
				st[nbest].CategoryID[k] = atoi(CategoryID);
				st[nbest].WordID[k] = atoi(WordID);
				strcpy(st[nbest].ElementText[k], WordText);
				//st[nbest].ElementConf[k] = ConfElement[nbest][j];
				
				/* �������P����o�� */
//				((CEdit*)GetDlgItem(IDC_EDIT_USER))->ReplaceSel(WordText);
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("�J�e�S��ID�F");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(CategoryID);
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("�P��ID�F");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(WordID);
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");
				
				k++;
			}
		}
		st[nbest].ElementsNum = k;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
// name:SplitWord(char *string, char a, char b,char *string1,char *string2, char *string3)
// �P��ɂ����^�O(�����l)�����o���֐�
//
//----------------------------------------------------------------------------------------
int CQueryByMouseDlg::SplitWord(char *string, char a, char b, 
			char *string1,
			char *string2, char *string3)
{
	//string�����͕����� , a,b...����a,b�ŕ���
	//string1���P��, string2���J�e�S��ID , string3���P��ID

	int i,j,k,l;
	int flag1 = 0;
	int flag2 = 0;

	//strlen(string)��������̒���
	for(i = 0, j = 0, k = 0,l = 0; i < (int)strlen(string); i++){

		if(string[i]==a){//'a'��������
			flag1 = 1;
		}else if(!flag1){//'a'���܂����ĂȂ������P��
			string1[j] = string[i];
			j++;
		}else{//'a'�������と�J�e�S��ID�{�P��ID

			if(string[i]==b){//'b'��������
				flag2 = 1;
			}else if(!flag2){//'b'���܂����ĂȂ������J�e�S��ID
				string2[k] = string[i];
				k++;
			}else{//'b'�������と�P��ID
				string3[l] = string[i];
				l++;
			}
		}
	}
	string1[j]='\0';
	string2[k]='\0';
	string3[l]='\0';

	return flag1;
	return flag2;
}

//--------------------------------------------------------------
//�V�X�e���̔��b�������[��
//
//��ԃt���O�ƃ^�X�N�t���O���甭�b�𐶐�����
//
//Input:�X���b�g�I�u�W�F�N�g
//Output:�Ȃ�
//---------------------------------------------------------------
void CQueryByMouseDlg::GenerateSystemMsg(CQuerySlot &Q)
{
	static int disnum=0;
	static bool f=false;
	//Set_xml("blank");

	//�^�X�N�����܂��Ă��Ȃ�
	if(taskflag==0){
		DispSystemmsg("������x���b���Ă�������");
		//AquesTalkDa_PlaySync("���������ǂ͂킵�Ă�������");
	}

	else{ //�^�X�N�����܂��Ă���

		//��F�����胂�[�h(��ł������������Ƃ܂Ƃ߂�)
		if(Q.GetCorNum()==1){ //���߃~�X
			if(endflag==true){ //�ĉ���
			}
			else if(endflag!=true){
				if(f==true){
					if(Q.GetObjectNum()>0){
					SceneOut.ShowWindow(SW_HIDE);
					DispSystemmsg("���̃V�[���ł�낵���ł����H");
					dummy.ShowWindow(SW_SHOW);
					dummy.ShowResult(Q.GetObjectNum());   //�摜�o��
					system.SetResult();
					}else{
						so->Stop();
						Sleep(200);
						SceneOut.ShowWindow(SW_HIDE);
						Q.SetCorNum(2);   //�����o���[�h
					}
			}else if(Q.GetTimeOK()==true){
				f=true;
				tflag = taskflag;
				total_obj_num = Database.Get_total_num();
				to = Database.Get_total_info();
				//�V�[�����̊l��
				if(taskflag==4){	//���u��
					//�u����Ă������̏��
					leftnum = Database.Get_leftnum();
					LtData1 = Database.Get_ltnum_info();
					int j=0;
					for(int i=0;i<leftnum;i++){
						if(LtData1[leftnum-i-1].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					leftnum=j;
					j=0;
					//�����蕨�̏��
					mnum = Database.Get_mnum();
					MData1 = Database.Get_mnum_info();
					for(int i=0;i<mnum;i++){
						if(MData1[i].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					mnum=j;
					j=0;
					//MData�ɕK�v�ȃf�[�^���W�߂�
					for(int i=0;i<leftnum;i++){
						for(int m=0;m<mnum;m++){
							if(MData1[m].frame < LtData1[i].frame){
								for(int j=mnum;j>m;j--)
									MData1[j] = MData1[j-1];
								MData1[m] = LtData1[i];
								mnum++;
								break;
							}
							else if(m==mnum-1){
								MData1[mnum] = LtData1[i];
								mnum++;
								break;
							}
						}
					}
				}else if(taskflag==7){ //������
					//�u���Ă��镨�̏��
					lnum = Database.Get_lnum();
					LData1 = Database.Get_lnum_info();
					int j=0;
					for(int i=0;i<lnum;i++){
						if(LData1[lnum-i-1].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					lnum=j;
					j=0;
					//�u����Ă������̏��
					leftnum = Database.Get_leftnum();
					LtData1 = Database.Get_ltnum_info();
					for(int i=0;i<leftnum;i++){
						if(LtData1[leftnum-i-1].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					leftnum=j;
					j=0;
					//LData�ɕK�v�ȃf�[�^���W�߂�
					for(int i=0;i<leftnum;i++){
						for(int l=0;l<lnum;l++){
							if(LData1[l].frame < LtData1[i].frame){
								for(int j=lnum;j>l;j--)
									LData1[j].frame = LData1[j-1].frame;
								LData1[l] = LtData1[i];
								lnum++;
								break;
							}
							else if(l==lnum-1){
								LData1[lnum] = LtData1[i];
								lnum++;
								break;
							}
						}
					}
				}
				if((taskflag==7 && lnum!=0) || (taskflag==4 && mnum!=0)){
					//�V�[�����\��
					SceneOut.ShowWindow(SW_SHOW);
					//�������o��
					AfxBeginThread(ThreadMDisp,&SceneOut,THREAD_PRIORITY_BELOW_NORMAL);
					DispSystemmsg("���̒��ɒT���Ă���V�[���͂���܂����H\r\n\r\n����Δԍ��������Ă�������");
					system.SetScene();
				}
				else{
					Q.SetCorNum(2);  //�����o���[�h
					Q.SetObjectNum(-1);
				}
			}
			if(Q.GetTimeOK()!=true){
				DispSystemmsg("�ߌ�T���ł�낵���ł����H");
				//AquesTalkDa_PlaySync("����,<NUM VAL=3 COUNTER=��>����/��낵'���ł����H");
				system.SetTime();
			}
		}
		}
		//+++++++++++++++++++++++++++++++//
		//       �����o�V�[������        //
		//+++++++++++++++++++++++++++++++//
		if(Q.GetCorNum()==2){
			if(endflag==true){ //�ĉ���
			}
			if(endflag!=true){
				//DispSystemmsg("�����o����");
				if(Q.GetObjectNum()<0 && CandNum>1){  //�������
					SceneOut.ShowWindow(SW_SHOW);
					//�X���b�h��p���ăJ��������摜���擾
					AfxBeginThread(ThreadMDispUndetection,&SceneOut,THREAD_PRIORITY_BELOW_NORMAL);
					DispSystemmsg("���̒��ɒT���Ă���V�[���͂���܂����H\r\n\r\n����Δԍ��������Ă�������");
					system.SetScene();
				}
				if(Q.GetObjectNum()>=0){
					SceneOut.ShowWindow(SW_HIDE);
					DispSystemmsg("���̃V�[���ł�낵���ł����H");
					dummy.ShowWindow(SW_SHOW);
					//dummy.ShowResult(Q.GetObjectNum());   //�摜�o��
					dummy.ShowUndetectioScene(CandFolder[Q.GetObjectNum()],CandFrame[Q.GetObjectNum()]);  //�V�[���o��
					system.SetResult();
				}
			}
			else{ //�����o�V�[���̊m�F����ꂽ�ꍇ
				//���̃V�[����Event-DB�ɒǉ�����
				int frame = CandFrame[Q.GetObjectNum()];

				//�t�H���_�쐬
				FileOperator path;
				char folder_name[30],tmp[10];
				sprintf_s(folder_name,"./Detected-Object/");
				sprintf_s(tmp,"%d",CandFolder[Q.GetObjectNum()]);
				strcat_s(folder_name,tmp);
				path.MakeDir(folder_name);

				//���̔ԍ����X�V
				Q.SetObjectNum(frame);
			}
		}
		
		if(Q.GetCorNum()==0){
			//����OK�܂�
			if(Q.GetTimeNum()!=0 && Q.GetTimeOK()!=true){
				DispSystemmsg("�ߌ�T���ł�낵���ł����H");
				//AquesTalkDa_PlaySync("����,<NUM VAL=3 COUNTER=��>����/��낵'���ł����H");
				system.SetTime();
			}


			//���̂�����ł��Ă���ꍇ
			if(Q.GetObjectOK()==true && endflag!=true){
				//++++++++++++++++++++++++
				// ���ʂ��o��
				//++++++++++++++++++++++++
				DispSystemmsg("���̐l����Ȃ��ł����H");
				//AquesTalkDa_PlaySync("���̂ЂƂ���Ȃ��ł���");
				dummy.ShowWindow(SW_SHOW);
				dummy.ShowResult(Q.GetObjectNum());   //�摜�o��
				system.SetResult();
			}

			//���̂�����ł��Ă��Ȃ��ꍇ
			if(Q.GetObjectOK()==false){
				//�ꏊ���������Ă��Ȃ��ꍇ
				if(!Q.GetArea()){
					//+++++++++++++++++++++
					// �ꏊ��q�˂�
					//+++++++++++++++++++++
					DispSystemmsg("�ʒu���w�����Ă�������");
					dummy.ShowWindow(SW_HIDE);
					//AquesTalkDa_PlaySync("��������т����Ă�������");
					//system.SetArea();
				}

				//�ꏊ���������Ă���ꍇ
				if(Q.GetArea()){
					//+++++++++++++++++++++
					// �ꏊ�̊m�F������
					//+++++++++++++++++++++
					if(Q.GetCandObjNum()<=0){  //���̂̌�₪�Ȃ��ꍇ
						//if(system.GetArea()==false){  //�ꏊ�̊m�F�����ĂȂ�
						if(Q.GetAreaOK()==false){
							DispSystemmsg("�ꏊ�͂����ł����Ă��܂���?");
							//AquesTalkDa_PlaySync("�΂���͂����ł����Ă��܂���");
							dummy.ShowWindow(SW_SHOW);
							dummy.DispAreaAlt(cp->getImage(), Q.GetArea_xmin(),Q.GetArea_xmax(),Q.GetArea_ymin(),Q.GetArea_ymax());
							
							system.SetArea();
							//Set_xml("ans");
						}else{  //�ꏊ�̊m�F������
							//++++++++++++++++++++//
							//   ��F���̔���     //
							//++++++++++++++++++++//
							dummy.ShowWindow(SW_HIDE);
							DispSystemmsg("��F�������I");
							Q.SetCorNum(1);  //�V�X�e���̌���߂�T���t���O
							//�����o�V�[���������n�߂�
							s.Set_Area(Q.GetArea_xmin(),Q.GetArea_xmax(),Q.GetArea_ymin(),Q.GetArea_ymax());
							AfxBeginThread(ThreadRefHuman,&query,THREAD_PRIORITY_BELOW_NORMAL);
						}
					}
					//+++++++++++++++++++++++
					// ���̊m�F������
					//+++++++++++++++++++++++
					else if(Q.GetCandObjNum()<5 || Q.GetTimeOK()==true){  //���̂̌�₪���ȉ�(10��)�̏ꍇ
						if(taskflag>6){
						//���o��
						//SetDlgItemText(IDC_EDIT2, "");       //���
						for(int i=0;i<Q.GetCandObjNum();i++){
							//char *x;
							//itoa(i+1,x,10);
						/*	((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(", ");

							itoa(Database.Disp_tmp(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("��");

							itoa(Database.Disp_tmp2(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("��");
							
							if(taskflag>=4 && taskflag<=6){
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" �������� ���m");
							}else{
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" �������� ���m");
							}

							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");*/
						}
						//char *x;
						//itoa(Q.GetObjectNum(),x,10);
						//disnum=Database.Disp_tmp3();
						disnum=Database.Give_Value();
						//itoa(disnum,x,10);
						//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("");
						//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("frame:");
						//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
						Q.SetObjectNum(disnum);

						dummy.ShowWindow(SW_SHOW);
						dummy.indicate(disnum,taskflag);   //�摜�o��

						//�Ƃ肠�����A�R�����g�A�E�g
						//if(Q.GetTimeOK()==true){
							//DispSystemmsg("3���O���T���܂�\r\n");
						//}
						DispSystemmsg("����̂��Ƃł����H");
						//AquesTalkDa_PlaySync("����̂��Ƃł���");
						system.SetObject();
						//Set_xml("ans");
						}
						else{
							//���o��
						//SetDlgItemText(IDC_EDIT2, "");       //���
						for(int i=0;i<Q.GetCandObjNum();i++){
							//char *x;
						/*	itoa(i+1,x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(", ");

							itoa(Database.Disp_Onj(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("��");

							itoa(Database.Disp_Onj2(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("��");
							
							if(taskflag>=4 && taskflag<=6){
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" �������� ���m");
							}else{
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" �������� ���m");
							}
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");*/
						}
						dummy.ShowWindow(SW_SHOW);
						dummy.indicate(Q.GetObjectNum(),taskflag);   //�摜�o��
						DispSystemmsg("����̂��Ƃł����H");
						//AquesTalkDa_PlaySync("����̂��Ƃł���");
						system.SetObject();
						//Set_xml("ans");
						}
					}

					//++++++++++++++++++++++
					// ���Ԃ�q�˂�
					//++++++++++++++++++++++
					if(Q.GetCandObjNum()>=5 && Q.GetTimeNum()==0){  //���̂̌�₪���ȏ�̏ꍇ
						//���o��
				/*		SetDlgItemText(IDC_EDIT2, "");       //���
						for(int i=0;i<Q.GetCandObjNum();i++){
							char *x;
							itoa(i+1,x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(", ");

							itoa(Database.Disp_Onj(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("��");

							itoa(Database.Disp_Onj2(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("��");
							
							if(taskflag>=4 && taskflag<=6){
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" �������� ���m");
							}else{
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" �������� ���m");
							}

							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");*/
						//}
						DispSystemmsg("��₪�������񂠂�܂��B");
						DispSystemmsg("�������炢�܂ł��������m���Ă܂����H");
						DispSystemmsg("����͉����ɂ���܂������H");
						//AquesTalkDa_PlaySync("����͂Ȃ񂶂ɂ���܂�����");
						system.SetTime();
						//Set_xml("time");
					}
					if(Q.GetCorNum()==1 && Q.GetTimeNum()==0){ //��F�����胂�[�h�ŁA����Not�m�莞
						if(taskflag==4){ //���u��
							DispSystemmsg("���ꂪ�����������Ԃ͕�����܂����H");
							//AquesTalkDa_PlaySync("����͂Ȃ񂶂ɂ���܂�����");
						}else if(taskflag==7){ //������
							DispSystemmsg("��������͂���܂������H");
							//AquesTalkDa_PlaySync("����͂Ȃ񂶂ɂ���܂�����");
						}
						system.SetTime();
						//Set_xml("time");
					}
				}
			}
			}
	}

	//�����I���̏ꍇ
	if(endflag==true){
		endflag=false;             //�I���t���O��߂�
		dummy.ShowWindow(SW_HIDE); //���ʃ_�C�A���O����
		Q.InitSlot();              //�X���b�g�̏�����
		PreSlot.InitSlot();        //�X���b�g�̏�����
		taskflag=0;
		system.InitContents();
		user.InitContents();
		//DispTimemsg();             //�����̍X�V
		//Set_xml("blank");
		//SetDlgItemText(IDC_EDIT2, "");
		SetDlgItemText(IDC_EDIT_USER, ""); 
		SetDlgItemText(IDC_EDIT_SYSTEM, "");
		Database.InitQuery();
		record=0;	
	}
}

//�}�E�X���N���b�N
//���͍��W���킹�������ɂ���Ă���
void CQueryByMouseDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);

	pointx = point.x;
	pointy = abs(240-(point.y-203));

	char msg[4];

	itoa(pointx,msg,10);  //10�i���ŕϊ�
	((CEdit*)GetDlgItem(IDC_X))->ReplaceSel("\r\n");
	((CEdit*)GetDlgItem(IDC_X))->ReplaceSel(msg);
	itoa(pointy,msg,10);
	((CEdit*)GetDlgItem(IDC_Y))->ReplaceSel("\r\n");
	((CEdit*)GetDlgItem(IDC_Y))->ReplaceSel(msg);
}

//-------------------------------------------------
// Name     : ImageOut(unsigned char *pix)
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2007.06.29
// Function : ���A���^�C���摜�o��
// Argument : pix:�o�͉摜�f�[�^
// return   : �Ȃ�
//-------------------------------------------------
void CQueryByMouseDlg::ImageOut(unsigned char *pix)
{
	//�摜�o��
	::BeginPaint(hwndImage,&ps);
		SetStretchBltMode(hdcImage,COLORONCOLOR);
		StretchDIBits(hdcImage,0,0,320,240,0,0,320,240,
		pix,&bmpInfo,DIB_RGB_COLORS,SRCCOPY);
	::EndPaint(hwndImage,&ps);
}

//-------------------------------------------------
// Name     : OnBnClickedOffline()
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2006.07.26
// Function : �I�t���C���ŏ���
// Argument : �Ȃ�
// return   : �Ȃ�
//-------------------------------------------------
void CQueryByMouseDlg::OnBnClickedOffline()
{
	//���̃N���X�̃|�C���^��n��
	camera.set(this);

	//�X���b�h��p���ăJ��������摜���擾
	AfxBeginThread(ThreadOffLine,&camera,THREAD_PRIORITY_BELOW_NORMAL);
}

//-------------------------------------------------
// Name     : Init()
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2007.06.29
// Function : ������
// Argument : �Ȃ�
// return   : �Ȃ�
//-------------------------------------------------
void CQueryByMouseDlg::Init()
{
	bmpInfo.bmiHeader.biHeight        = 240;
	bmpInfo.bmiHeader.biWidth         = 320;
	bmpInfo.bmiHeader.biBitCount      = 24;
	bmpInfo.bmiHeader.biClrImportant  = 0;
	bmpInfo.bmiHeader.biClrUsed       = 0;
	bmpInfo.bmiHeader.biCompression   = 0;
	bmpInfo.bmiHeader.biPlanes        = 1;
	bmpInfo.bmiHeader.biSize          = 40;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;

	hwndImage = NULL;
	GetDlgItem(IDC_IMAGE,&hwndImage);
	hdcImage = ::GetDC(hwndImage);

	record=0;
}

//�N���u�����H
void CQueryByMouseDlg::OnBnClickedPut()
{
	DispUsermsg("�N���u�����H");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","�u����#07$001,");

	record++;
	ControlDialog();
}

//�N�������Ă������H
void CQueryByMouseDlg::OnBnClickedLeave()
{
	DispUsermsg("�N�������Ă������H");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","�����Ă�����#08$001,");

	record++;
	ControlDialog();
}

//�m��u�͂��v
void CQueryByMouseDlg::OnBnClickedYes()
{
	DispUsermsg("�͂�");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","�͂�#09$001,");

	record++;
	ControlDialog();
}

//�ے�u�������v
void CQueryByMouseDlg::OnBnClickedNo()
{
	DispUsermsg("������");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","������#10$001,");

	record++;
	ControlDialog();
}

//���Ӂu���肪�Ƃ��v
void CQueryByMouseDlg::OnBnClickedThanks()
{
	DispUsermsg("���肪�Ƃ�");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","���肪�Ƃ�#13$001,");

	record++;
	ControlDialog();
}

//����
void CQueryByMouseDlg::OnBnClickedTime()
{
	CString str;
	UpdateData(TRUE);
	m_xcTime.GetLBText(m_xvTime,str);
	//MessageBox(str);
	str += "��";

	//�Ƃ肠�����A���ԌŒ��
	DispUsermsg((LPCSTR)(str + "���炢"));
	st[0].ElementsNum = 1;
	str += "#19$003,";
	sprintf_s(Word[0],str);

	record++;
	ControlDialog();
}

//�ԍ�
void CQueryByMouseDlg::OnBnClickedNum()
{
	CString str,tid;
	UpdateData(TRUE);
	m_xcTime.GetLBText(m_xvTime,str);

	int id = atoi(str);
	tid.Format("%02d",id);

	char *pstr = new char[str.GetLength()+1];
	strcpy_s(pstr,str.GetLength()+1,str);
	str += "��";
	DispUsermsg((LPCSTR)str);

	str += "#100$0" + tid + ",";
	
	st[0].ElementsNum = 1;
	strcpy(Word[0],str);
	//sprintf_s(Word[0],"%s","1��#100$001,");

	record++;
	ControlDialog();
}

//ONLINE
void CQueryByMouseDlg::OnBnClickedOnline()
{
	//���̃J�������g�p���̏ꍇ
	if(CameraNumber>0){
		cp->set_stop();
		Sleep(200);
	}

	CameraNumber = 3;  //�g�p�J�����̐ݒ�

	//���̃N���X�̃|�C���^��n��
	camera.set_pParent(this);

	//�X���b�h��p���ăJ��������摜���擾
	AfxBeginThread(ThreadProcCalc,&camera,THREAD_PRIORITY_BELOW_NORMAL);
}
