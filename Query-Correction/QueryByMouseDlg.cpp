// QueryByMouseDlg.cpp : 実装ファイル
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


CCamera camera;   //カメラ制御クラス
CCamera *cp;      //カメラ制御クラス(スレッド用)
int CameraNumber;  //使用カメラ番号

Multi_Disp SceneOut;  //誤認識時のシーン出力用ダイアログ
Multi_Disp *so;
//再解釈用?+++++++++++++++++++++++++++++++++++++
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
int Qtime=-1;       //検索用の時刻
CdispUndetection dp;

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// CQueryByMouseDlg ダイアログ





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

//スレッド(オンライン)
static UINT AFX_CDECL ThreadProcCalc(LPVOID pParam)
{
	cp=(CCamera*)pParam;
	cp->SendCommand(CameraNumber,5);
	return 0;
}

//スレッド(オフライン)
static UINT AFX_CDECL ThreadOffLine(LPVOID pParam)
{
	cp=(CCamera*)pParam;
	cp->imageKinect();
	//cp->OffLine();
	return 0;
}

//誤解釈候補シーン表示用スレッド
static UINT AFX_CDECL ThreadMDisp(LPVOID pParam)
{
	so=(Multi_Disp*)pParam;
	if(tflag==4) //放置物
		so->indicate(MData1,mnum);
	else if(tflag==7) //紛失物
		so->indicate(LData1,lnum);

	return 0;
}

//未検出候補シーン表示用スレッド
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

//スレッド(未検出シーンを検索--最新から過去に向かって)
static UINT AFX_CDECL ThreadSeek(LPVOID pParam)
{
	sp=(CseekUndetection*)pParam;
	for(int i=0;i<SceneNum;i++){
		if(query.Get_hour(i)>=Qtime){
			bool sflag=true;
			CandFrame[CandNum]=s.Reference(query.Get_sframe(i),query.Get_eframe(i));
			if(CandFrame[CandNum]!=0){
				//Event-DB内のシーンと同じものは候補にしない
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
					//優先順位入れ替えするべきだろうけど、今はせず
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

//スレッド(HumanDB検索)
static UINT AFX_CDECL ThreadRefHuman(LPVOID pParam)
{
	hp=(CqueryHuman*)pParam;
	SceneNum = hp->Reference();
	if(SceneNum>0)
		AfxBeginThread(ThreadSeek,&s,THREAD_PRIORITY_BELOW_NORMAL);
	return 0;
}


// CQueryByMouseDlg メッセージ ハンドラ

BOOL CQueryByMouseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	Init();
	CameraNumber=0;  //使われているカメラの番号(初期化)
	dummy.Create(IDD_OUTPUT,NULL);
	SceneOut.Create(IDD_MULTI_DISP,NULL);

	SceneOut.ShowWindow(SW_SHOW);

	//リスト
	m_xcTime.InsertString(-1,"1");	m_xcTime.InsertString(-1,"2");	m_xcTime.InsertString(-1,"3");
	m_xcTime.InsertString(-1,"4");	m_xcTime.InsertString(-1,"5");	m_xcTime.InsertString(-1,"6");
	m_xcTime.InsertString(-1,"7");	m_xcTime.InsertString(-1,"8");	m_xcTime.InsertString(-1,"9");
	m_xcTime.InsertString(-1,"10");	m_xcTime.InsertString(-1,"11");	m_xcTime.InsertString(-1,"12");

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CQueryByMouseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CQueryByMouseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//---------------------------------------------------------
// Name:DispSystemmsg()
//「システムの発話」にメッセージを表示する関数
//
//Input:string型の文字列
//Outout:ダイアログの画面にそのIDの場所に文字列表示
//---------------------------------------------------------
void CQueryByMouseDlg::DispSystemmsg(const std::string &msg)
{
	SetDlgItemText(IDC_EDIT_SYSTEM, "");   //ユーザ発話
	((CEdit*)GetDlgItem(IDC_EDIT_SYSTEM))->ReplaceSel(msg.c_str());
}

//---------------------------------------------------------
// NAME:DispUsermsg()
//「ユーザの発話」にメッセージを表示する関数
//
//Input:string型の文字列
//Outout:ダイアログの画面にそのIDの場所に文字列表示
//---------------------------------------------------------
void CQueryByMouseDlg::DispUsermsg(const std::string &msg)
{
	SetDlgItemText(IDC_EDIT_USER, "");   //ユーザ発話
	((CEdit*)GetDlgItem(IDC_EDIT_USER))->ReplaceSel(msg.c_str());
}

//------------------------------------------------------------
// Name:ControlDialog() [作成者：片山　2004年未明]
//                      [改良者：牧　]
//  
// 対話制御部：ここで認識結果を用いて制御していく
// Input:なし
//Output:なし
//------------------------------------------------------------
void CQueryByMouseDlg::ControlDialog()
{
	//###########################################################
	//ここから下で対話制御 「対話制御の手順」
	// 1.認識結果を認識構造体に分けて入れる
	// 2.タスクを生成
	// 3.スロットオブジェクトに代入
	//   3-1.タスクのスロット構造に入れるために認識結果の補完
	//   3-2.タスクのスロット構造に入れるために認識結果の推定
	//   3-3.ユーザの発話フラグを立てる
	// 4.システム発話生成
	//   4-1.スロットの状態＋ユーザの発話フラグから発話文生成
	//   4-2.システム発話フラグを立てる
	//###########################################################

	// 1.まず認識結果を認識結果構造体に格納する
	SplitResult();//認識結果を認識構造体に分けて入れる
	//WriteResultLogFile();//ファイルに認識結果を書き込む

	// 2.キーワードから実行するタスクを決める
	//これはユーザの第１発話(recordが1)の時のみ行う
	if(record==1){

		//とりあえず、SetTask()をコメントアウト(4/15)
		SetTask();   //タスクの決定


		if(taskflag>=1){ //タスクがあれば
			//DispSystemmsg("---[問い合わせの受付]---\r\n");
		}else{ //タスクが未検出
			//DispSystemmsg("---[問い合わせ以外を生成できません]---\r\n");
		}
	}

	//3.スロットオブジェクトに代入
	IntoSlot(Qslot);  //スロット更新


	//4.システム発話生成 = スロットの情報+ユーザの発話フラグ
	system.InitContents();  //発話内容フラグの初期化
	GenerateSystemMsg(Qslot);//発話生成ルーチン
}

//--------------------------------------------------------------------------
//NAME:GessTask()
//[タスクの推定]
//Rule1:「いつ」や「誰」が見つかれば、問い合わせ
//Rule2: 動詞で判断：「置いた」、「持って来た」、「ある」は放置物
//                   「持って行った」、「無くなった」は紛失物
//
// Input:認識結果構造体 st 、int型 N-best
// Output:int型のフラグgesstask
//--------------------------------------------------------------------------
int CQueryByMouseDlg::GessTask()
{
	int gesstask = 0;//タスク推定フラグ：0は未検出、1は問い合わせ(人)、2は問い合わせ(時間)、3は問い合わせ(人、時間)
					 //					 4は放置物(人)、5は放置物(時間)、6は放置物(人、時間)
	                 //                  7は紛失物(人)、8は紛失物(時間)、9は紛失物(人、時間)
	bool hito=false;
	bool time=false;

	for(nbest = 0 ; nbest < MAX_CAND_NUM ; nbest++){//候補数だけ繰り返す
		for(int i = 0; i < st[nbest].ElementsNum ;i++){
			if(st[nbest].CategoryID[i]==5){  //「いつ」が見つかれば
				time = true;
			}else if(st[nbest].CategoryID[i]==6){ //「誰」が見つかれば
				hito = true;
			}else if(st[nbest].CategoryID[i]==7){ //放置物の問い合わせ
				gesstask = 4;
			}else if(st[nbest].CategoryID[i]==8){ //紛失物の問い合わせ
				gesstask = 7;
			}
		}
	}

	//タスクの決定
	if(hito==true)	gesstask+=1;
	if(time==true)	gesstask+=2;

	if(gesstask == 0) {		
		record = 0;//キーワードが検出されるまで対話は進まないようにする
		return 0;
	}
	
	return gesstask;
}

//------------------------------------------------------------------------
//NAME:IntoSlot()
//問い合わせスロットを埋めていく
//
//
//-----------------------------------------------------------------------
int CQueryByMouseDlg::IntoSlot(CQuerySlot &Q)
{
	bool AreaFlag=false;

	//if(pointx)	AreaFlag=true;

	nbest = 0;//第一候補の認識結果のみを扱うため0にしておく

	if(taskflag==0)	return 0;

	//感謝の言葉なら終了(初期化) 
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==13){
			endflag=true;
		}
	}

	//時間
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==19){
			Q.SetTime(st[nbest].WordID[i]+12);
			//Q.SetTime(12);
			return 0;
		}
	}

	//番号
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==100){
			so->Stop();
			Sleep(200);
			//誤解釈モード
			if(Q.GetCorNum()==1){
				if(taskflag==4)
					Q.SetObjectNum(MData1[st[nbest].WordID[i]-1].frame);
				else if(taskflag==7)
					Q.SetObjectNum(LData1[st[nbest].WordID[i]-1].frame);
			} 
			//未検出モード
			else if(Q.GetCorNum()==2){
				Q.SetObjectNum(st[nbest].WordID[i]-1);
			}
		}
	}

	//返答が肯定の場合
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==9){
			//
			user=system;
			Q = PreSlot;

			if(system.GetObject()){  //システムが物体について確認をしていた場合
				Q.SetObjectOK(true);     //物体OK
			}
			if(system.GetArea()){ //システムが場所について確認をしていた場合
				Q.SetAreaOK(true);    //場所OK
			}
			if(system.GetResult()){ //システムが結果を見せた後
				endflag=true;
			}
			if(system.GetTime()){   //時間確認
				Q.SetTimeOK(true);
				if(Q.GetCorNum()!=1){ //普通の検索なら時間で絞る
					int num=Database.NarrowByTime(Q.GetTimeNum());
					Q.SetCandObjNum(num);
					Q.SetObjectNum(Database.Give_Value());
				}
			}
		}
	}

	//返答が否定の場合
	for(int i=0;i<st[nbest].ElementsNum;i++){
		if(st[nbest].CategoryID[i]==10){
			if(system.GetObject()){  //システムが物体について確認をしていた場合
				//物体候補の更新
				Database.Renewal(); //検索の更新
				Q.SetCandObjNum(Q.GetCandObjNum()-1); //候補数格納
				if(Q.GetCandObjNum()>0)
					Q.SetObjectNum(Database.Give_Value()); //最も優先度が高い物体の番号
			}
			if(system.GetArea()){  //システムが場所について確認をしていた場合
				Q.SetArea_xmax(0); Q.SetArea_xmin(0);
				Q.SetArea_ymax(0); Q.SetArea_ymin(0);
			}
			if(system.GetScene()){ //システムが誤解釈・未検出候補シーンを提示した場合
				so->Stop();
				Sleep(200);
				Q.SetObjectNum(-1);
				Q.SetCorNum(Q.GetCorNum()+1);
			}
		}
	}

	//++++++++++++++++++++++++++++++++++++++++++
	// ユーザが指差している領域を調べる
	// 1.場所がまだ確定していない場合
	// 2.システムが指差しを要求した場合
	// 3.ユーザが場所の訂正をした場合
	//
	//++++++++++++++++++++++++++++++++++++++++++
	int count = 0;
	if(Q.GetArea()==0 && system.GetArea()!=true){ //場所がまだ確定していない
		while(count < 10){
			AreaFlag=ReferenceObject();
			if (AreaFlag) count++;
			Sleep(10);
		}
		if(count >= 10){ //場所が分かったとき、スロット更新
			Q.SetArea_xmin(xmin[0]); Q.SetArea_xmax(xmax[0]);
			Q.SetArea_ymin(ymin[0]); Q.SetArea_ymax(ymax[0]);
		//	Q.SetArea_xmin(pointx-15); Q.SetArea_xmax(pointx+15);
		//	Q.SetArea_ymin(pointy-15); Q.SetArea_ymax(pointy+15);
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++
	// 検索して候補を探す
	//
	//+++++++++++++++++++++++++++++++++++++++++
	if(Q.GetArea()!=0 && Q.GetCandObjNum()==0 && system.GetObject()!=true && system.GetArea()!=true){ //初めての検索
		int num=Database.Reference(xmin[0],ymin[0],xmax[0],ymax[0],taskflag);
		//int num=Database.Reference(pointx-10,pointy-10,pointx+10,pointy+10,taskflag);
		if(num>0){ //候補があれば
			Q.SetCandObjNum(num); //候補数格納
			Q.SetObjectNum(Database.Give_Value()); //最も優先度が高い物体の番号
		}
	}

	PreSlot = Q;  //１発話前のスロットの情報を保存しておく．ルールによっては書き換わるかもしれないから

	return 1;
}


//--------------------------------------------------------------
// name:ReferenceObject()
// ユーザの指さす領域を得る
//
//Input:
//Output:なし
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
//認識結果を認識構造体に分けて入れる
//
// Input:なし
//Output:int型
//-------------------------------------
int CQueryByMouseDlg::SplitResult()
{
	char WordText[MAX_CHAR_NUM_DIC];//品詞
	char CategoryID[MAX_CATID_NAME];//カテゴリID
	char WordID[MAX_WORDID_NAME];//語句ID

	st[0].CategoryID=(int*)calloc(MAX_ELEMENT_NUM,sizeof(int));
	st[0].WordID=(int*)calloc(MAX_ELEMENT_NUM,sizeof(int));
	st[0].ElementConf=(float*)calloc(MAX_ELEMENT_NUM,sizeof(float));
	st[0].ElementText=(char**)balloc(MAX_ELEMENT_NUM,MAX_CHAR_NUM_DIC,sizeof(char));

	for(nbest = 0 ; nbest < MAX_CAND_NUM ; nbest++){//候補数だけ繰り返す
		int k = 0;
		for(int j=0; j<st[nbest].ElementsNum; j++){
			if(SplitWord(Word[j], '#', '$', WordText, CategoryID , WordID)){
				st[nbest].CategoryID[k] = atoi(CategoryID);
				st[nbest].WordID[k] = atoi(WordID);
				strcpy(st[nbest].ElementText[k], WordText);
				//st[nbest].ElementConf[k] = ConfElement[nbest][j];
				
				/* 分けた単語を出力 */
//				((CEdit*)GetDlgItem(IDC_EDIT_USER))->ReplaceSel(WordText);
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("カテゴリID：");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(CategoryID);
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");
				//((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("単語ID：");
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
// 単語についたタグ(属性値)を取り出す関数
//
//----------------------------------------------------------------------------------------
int CQueryByMouseDlg::SplitWord(char *string, char a, char b, 
			char *string1,
			char *string2, char *string3)
{
	//string＝入力文字列 , a,b...文字a,bで分割
	//string1＝単語, string2＝カテゴリID , string3＝単語ID

	int i,j,k,l;
	int flag1 = 0;
	int flag2 = 0;

	//strlen(string)＝文字列の長さ
	for(i = 0, j = 0, k = 0,l = 0; i < (int)strlen(string); i++){

		if(string[i]==a){//'a'が来たら
			flag1 = 1;
		}else if(!flag1){//'a'がまだ来てない時→単語
			string1[j] = string[i];
			j++;
		}else{//'a'が来た後→カテゴリID＋単語ID

			if(string[i]==b){//'b'がきたら
				flag2 = 1;
			}else if(!flag2){//'b'がまだ来てない時→カテゴリID
				string2[k] = string[i];
				k++;
			}else{//'b'が来た後→単語ID
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
//システムの発話生成ルール
//
//状態フラグとタスクフラグから発話を生成する
//
//Input:スロットオブジェクト
//Output:なし
//---------------------------------------------------------------
void CQueryByMouseDlg::GenerateSystemMsg(CQuerySlot &Q)
{
	static int disnum=0;
	static bool f=false;
	//Set_xml("blank");

	//タスクが決まっていない
	if(taskflag==0){
		DispSystemmsg("もう一度発話してください");
		//AquesTalkDa_PlaySync("もういちどはつわしてください");
	}

	else{ //タスクが決まっている

		//誤認識特定モード(後でもう少しちゃんとまとめる)
		if(Q.GetCorNum()==1){ //解釈ミス
			if(endflag==true){ //再解釈
			}
			else if(endflag!=true){
				if(f==true){
					if(Q.GetObjectNum()>0){
					SceneOut.ShowWindow(SW_HIDE);
					DispSystemmsg("このシーンでよろしいですか？");
					dummy.ShowWindow(SW_SHOW);
					dummy.ShowResult(Q.GetObjectNum());   //画像出力
					system.SetResult();
					}else{
						so->Stop();
						Sleep(200);
						SceneOut.ShowWindow(SW_HIDE);
						Q.SetCorNum(2);   //未検出モード
					}
			}else if(Q.GetTimeOK()==true){
				f=true;
				tflag = taskflag;
				total_obj_num = Database.Get_total_num();
				to = Database.Get_total_info();
				//シーン候補の獲得
				if(taskflag==4){	//放置物
					//置かれていた物体情報
					leftnum = Database.Get_leftnum();
					LtData1 = Database.Get_ltnum_info();
					int j=0;
					for(int i=0;i<leftnum;i++){
						if(LtData1[leftnum-i-1].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					leftnum=j;
					j=0;
					//持去り物体情報
					mnum = Database.Get_mnum();
					MData1 = Database.Get_mnum_info();
					for(int i=0;i<mnum;i++){
						if(MData1[i].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					mnum=j;
					j=0;
					//MDataに必要なデータを集める
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
				}else if(taskflag==7){ //紛失物
					//置いてある物体情報
					lnum = Database.Get_lnum();
					LData1 = Database.Get_lnum_info();
					int j=0;
					for(int i=0;i<lnum;i++){
						if(LData1[lnum-i-1].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					lnum=j;
					j=0;
					//置かれていた物体情報
					leftnum = Database.Get_leftnum();
					LtData1 = Database.Get_ltnum_info();
					for(int i=0;i<leftnum;i++){
						if(LtData1[leftnum-i-1].time.hour < Q.GetTimeNum())	break;
						j++;
					}
					leftnum=j;
					j=0;
					//LDataに必要なデータを集める
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
					//シーン候補表示
					SceneOut.ShowWindow(SW_SHOW);
					//複数候補出力
					AfxBeginThread(ThreadMDisp,&SceneOut,THREAD_PRIORITY_BELOW_NORMAL);
					DispSystemmsg("この中に探しているシーンはありますか？\r\n\r\nあれば番号を教えてください");
					system.SetScene();
				}
				else{
					Q.SetCorNum(2);  //未検出モード
					Q.SetObjectNum(-1);
				}
			}
			if(Q.GetTimeOK()!=true){
				DispSystemmsg("午後５時でよろしいですか？");
				//AquesTalkDa_PlaySync("ごご,<NUM VAL=3 COUNTER=時>じで/よろし'いですか？");
				system.SetTime();
			}
		}
		}
		//+++++++++++++++++++++++++++++++//
		//       未検出シーン検索        //
		//+++++++++++++++++++++++++++++++//
		if(Q.GetCorNum()==2){
			if(endflag==true){ //再解釈
			}
			if(endflag!=true){
				//DispSystemmsg("未検出検索");
				if(Q.GetObjectNum()<0 && CandNum>1){  //複数候補
					SceneOut.ShowWindow(SW_SHOW);
					//スレッドを用いてカメラから画像を取得
					AfxBeginThread(ThreadMDispUndetection,&SceneOut,THREAD_PRIORITY_BELOW_NORMAL);
					DispSystemmsg("この中に探しているシーンはありますか？\r\n\r\nあれば番号を教えてください");
					system.SetScene();
				}
				if(Q.GetObjectNum()>=0){
					SceneOut.ShowWindow(SW_HIDE);
					DispSystemmsg("このシーンでよろしいですか？");
					dummy.ShowWindow(SW_SHOW);
					//dummy.ShowResult(Q.GetObjectNum());   //画像出力
					dummy.ShowUndetectioScene(CandFolder[Q.GetObjectNum()],CandFrame[Q.GetObjectNum()]);  //シーン出力
					system.SetResult();
				}
			}
			else{ //未検出シーンの確認が取れた場合
				//そのシーンをEvent-DBに追加する
				int frame = CandFrame[Q.GetObjectNum()];

				//フォルダ作成
				FileOperator path;
				char folder_name[30],tmp[10];
				sprintf_s(folder_name,"./Detected-Object/");
				sprintf_s(tmp,"%d",CandFolder[Q.GetObjectNum()]);
				strcat_s(folder_name,tmp);
				path.MakeDir(folder_name);

				//物体番号も更新
				Q.SetObjectNum(frame);
			}
		}
		
		if(Q.GetCorNum()==0){
			//時間OKまだ
			if(Q.GetTimeNum()!=0 && Q.GetTimeOK()!=true){
				DispSystemmsg("午後５時でよろしいですか？");
				//AquesTalkDa_PlaySync("ごご,<NUM VAL=3 COUNTER=時>じで/よろし'いですか？");
				system.SetTime();
			}


			//物体が特定できている場合
			if(Q.GetObjectOK()==true && endflag!=true){
				//++++++++++++++++++++++++
				// 結果を出力
				//++++++++++++++++++++++++
				DispSystemmsg("この人じゃないですか？");
				//AquesTalkDa_PlaySync("このひとじゃないですか");
				dummy.ShowWindow(SW_SHOW);
				dummy.ShowResult(Q.GetObjectNum());   //画像出力
				system.SetResult();
			}

			//物体が特定できていない場合
			if(Q.GetObjectOK()==false){
				//場所が分かっていない場合
				if(!Q.GetArea()){
					//+++++++++++++++++++++
					// 場所を尋ねる
					//+++++++++++++++++++++
					DispSystemmsg("位置を指差してください");
					dummy.ShowWindow(SW_HIDE);
					//AquesTalkDa_PlaySync("いちをゆびさしてください");
					//system.SetArea();
				}

				//場所が分かっている場合
				if(Q.GetArea()){
					//+++++++++++++++++++++
					// 場所の確認をする
					//+++++++++++++++++++++
					if(Q.GetCandObjNum()<=0){  //物体の候補がない場合
						//if(system.GetArea()==false){  //場所の確認をしてない
						if(Q.GetAreaOK()==false){
							DispSystemmsg("場所はここであっていますか?");
							//AquesTalkDa_PlaySync("ばしょはここであっていますか");
							dummy.ShowWindow(SW_SHOW);
							dummy.DispAreaAlt(cp->getImage(), Q.GetArea_xmin(),Q.GetArea_xmax(),Q.GetArea_ymin(),Q.GetArea_ymax());
							
							system.SetArea();
							//Set_xml("ans");
						}else{  //場所の確認をした
							//++++++++++++++++++++//
							//   誤認識の発見     //
							//++++++++++++++++++++//
							dummy.ShowWindow(SW_HIDE);
							DispSystemmsg("誤認識発見！");
							Q.SetCorNum(1);  //システムの誤解釈を探すフラグ
							//未検出シーン検索を始める
							s.Set_Area(Q.GetArea_xmin(),Q.GetArea_xmax(),Q.GetArea_ymin(),Q.GetArea_ymax());
							AfxBeginThread(ThreadRefHuman,&query,THREAD_PRIORITY_BELOW_NORMAL);
						}
					}
					//+++++++++++++++++++++++
					// 物の確認をする
					//+++++++++++++++++++++++
					else if(Q.GetCandObjNum()<5 || Q.GetTimeOK()==true){  //物体の候補が一定以下(10個)の場合
						if(taskflag>6){
						//候補出力
						//SetDlgItemText(IDC_EDIT2, "");       //候補
						for(int i=0;i<Q.GetCandObjNum();i++){
							//char *x;
							//itoa(i+1,x,10);
						/*	((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(", ");

							itoa(Database.Disp_tmp(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("時");

							itoa(Database.Disp_tmp2(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("分");
							
							if(taskflag>=4 && taskflag<=6){
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" 持ち込み 検知");
							}else{
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" 持ち去り 検知");
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
						dummy.indicate(disnum,taskflag);   //画像出力

						//とりあえず、コメントアウト
						//if(Q.GetTimeOK()==true){
							//DispSystemmsg("3時前後を探します\r\n");
						//}
						DispSystemmsg("これのことですか？");
						//AquesTalkDa_PlaySync("これのことですか");
						system.SetObject();
						//Set_xml("ans");
						}
						else{
							//候補出力
						//SetDlgItemText(IDC_EDIT2, "");       //候補
						for(int i=0;i<Q.GetCandObjNum();i++){
							//char *x;
						/*	itoa(i+1,x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(", ");

							itoa(Database.Disp_Onj(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("時");

							itoa(Database.Disp_Onj2(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("分");
							
							if(taskflag>=4 && taskflag<=6){
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" 持ち込み 検知");
							}else{
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" 持ち去り 検知");
							}
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");*/
						}
						dummy.ShowWindow(SW_SHOW);
						dummy.indicate(Q.GetObjectNum(),taskflag);   //画像出力
						DispSystemmsg("これのことですか？");
						//AquesTalkDa_PlaySync("これのことですか");
						system.SetObject();
						//Set_xml("ans");
						}
					}

					//++++++++++++++++++++++
					// 時間を尋ねる
					//++++++++++++++++++++++
					if(Q.GetCandObjNum()>=5 && Q.GetTimeNum()==0){  //物体の候補が一定以上の場合
						//候補出力
				/*		SetDlgItemText(IDC_EDIT2, "");       //候補
						for(int i=0;i<Q.GetCandObjNum();i++){
							char *x;
							itoa(i+1,x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(", ");

							itoa(Database.Disp_Onj(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("時");

							itoa(Database.Disp_Onj2(),x,10);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(x);
							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("分");
							
							if(taskflag>=4 && taskflag<=6){
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" 持ち込み 検知");
							}else{
								((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel(" 持ち去り 検知");
							}

							((CEdit*)GetDlgItem(IDC_EDIT2))->ReplaceSel("\r\n");*/
						//}
						DispSystemmsg("候補がたくさんあります。");
						DispSystemmsg("何時ぐらいまであったか知ってますか？");
						DispSystemmsg("それは何時にありましたか？");
						//AquesTalkDa_PlaySync("それはなんじにありましたか");
						system.SetTime();
						//Set_xml("time");
					}
					if(Q.GetCorNum()==1 && Q.GetTimeNum()==0){ //誤認識特定モードで、時間Not確定時
						if(taskflag==4){ //放置物
							DispSystemmsg("それが無かった時間は分かりますか？");
							//AquesTalkDa_PlaySync("それはなんじにありましたか");
						}else if(taskflag==7){ //紛失物
							DispSystemmsg("いつ頃それはありましたか？");
							//AquesTalkDa_PlaySync("それはなんじにありましたか");
						}
						system.SetTime();
						//Set_xml("time");
					}
				}
			}
			}
	}

	//検索終了の場合
	if(endflag==true){
		endflag=false;             //終了フラグを戻す
		dummy.ShowWindow(SW_HIDE); //結果ダイアログ消す
		Q.InitSlot();              //スロットの初期化
		PreSlot.InitSlot();        //スロットの初期化
		taskflag=0;
		system.InitContents();
		user.InitContents();
		//DispTimemsg();             //時刻の更新
		//Set_xml("blank");
		//SetDlgItemText(IDC_EDIT2, "");
		SetDlgItemText(IDC_EDIT_USER, ""); 
		SetDlgItemText(IDC_EDIT_SYSTEM, "");
		Database.InitQuery();
		record=0;	
	}
}

//マウス左クリック
//今は座標合わせが微妙にずれている
void CQueryByMouseDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);

	pointx = point.x;
	pointy = abs(240-(point.y-203));

	char msg[4];

	itoa(pointx,msg,10);  //10進数で変換
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
// Function : リアルタイム画像出力
// Argument : pix:出力画像データ
// return   : なし
//-------------------------------------------------
void CQueryByMouseDlg::ImageOut(unsigned char *pix)
{
	//画像出力
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
// Function : オフラインで処理
// Argument : なし
// return   : なし
//-------------------------------------------------
void CQueryByMouseDlg::OnBnClickedOffline()
{
	//このクラスのポインタを渡す
	camera.set(this);

	//スレッドを用いてカメラから画像を取得
	AfxBeginThread(ThreadOffLine,&camera,THREAD_PRIORITY_BELOW_NORMAL);
}

//-------------------------------------------------
// Name     : Init()
// Author   : Kazuhiro MAKI (CV-lab.)
// Updata   : 2007.06.29
// Function : 初期化
// Argument : なし
// return   : なし
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

//誰が置いた？
void CQueryByMouseDlg::OnBnClickedPut()
{
	DispUsermsg("誰が置いた？");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","置いた#07$001,");

	record++;
	ControlDialog();
}

//誰が持っていった？
void CQueryByMouseDlg::OnBnClickedLeave()
{
	DispUsermsg("誰が持っていった？");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","持っていった#08$001,");

	record++;
	ControlDialog();
}

//肯定「はい」
void CQueryByMouseDlg::OnBnClickedYes()
{
	DispUsermsg("はい");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","はい#09$001,");

	record++;
	ControlDialog();
}

//否定「いいえ」
void CQueryByMouseDlg::OnBnClickedNo()
{
	DispUsermsg("いいえ");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","いいえ#10$001,");

	record++;
	ControlDialog();
}

//感謝「ありがとう」
void CQueryByMouseDlg::OnBnClickedThanks()
{
	DispUsermsg("ありがとう");
	st[0].ElementsNum = 1;
	sprintf_s(Word[0],"%s","ありがとう#13$001,");

	record++;
	ControlDialog();
}

//時間
void CQueryByMouseDlg::OnBnClickedTime()
{
	CString str;
	UpdateData(TRUE);
	m_xcTime.GetLBText(m_xvTime,str);
	//MessageBox(str);
	str += "時";

	//とりあえず、時間固定で
	DispUsermsg((LPCSTR)(str + "ぐらい"));
	st[0].ElementsNum = 1;
	str += "#19$003,";
	sprintf_s(Word[0],str);

	record++;
	ControlDialog();
}

//番号
void CQueryByMouseDlg::OnBnClickedNum()
{
	CString str,tid;
	UpdateData(TRUE);
	m_xcTime.GetLBText(m_xvTime,str);

	int id = atoi(str);
	tid.Format("%02d",id);

	char *pstr = new char[str.GetLength()+1];
	strcpy_s(pstr,str.GetLength()+1,str);
	str += "番";
	DispUsermsg((LPCSTR)str);

	str += "#100$0" + tid + ",";
	
	st[0].ElementsNum = 1;
	strcpy(Word[0],str);
	//sprintf_s(Word[0],"%s","1番#100$001,");

	record++;
	ControlDialog();
}

//ONLINE
void CQueryByMouseDlg::OnBnClickedOnline()
{
	//他のカメラが使用中の場合
	if(CameraNumber>0){
		cp->set_stop();
		Sleep(200);
	}

	CameraNumber = 3;  //使用カメラの設定

	//このクラスのポインタを渡す
	camera.set_pParent(this);

	//スレッドを用いてカメラから画像を取得
	AfxBeginThread(ThreadProcCalc,&camera,THREAD_PRIORITY_BELOW_NORMAL);
}
