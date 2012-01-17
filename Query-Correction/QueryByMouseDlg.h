// QueryByMouseDlg.h : ヘッダー ファイル
//

#pragma once

#include "network.h"
#include "bmp.h"

#include "AllocateMemory.h"
#include "constants.h"
#include "query.h"
#include "Output.h"
#include "QuerySlot.h"
#include "TalkedContents.h"

#include "Multi_Disp.h"
#include "afxwin.h"

//認識結果
typedef struct _Sentence{
	int ElementsNum;          //形態素数
	int* CategoryID;          //カテゴリID
	int* WordID;              //単語ID
	float* ElementConf;       //形態素の信頼度
	char** ElementText;       //形態素のテキスト
} mySentence;

// CQueryByMouseDlg ダイアログ
class CQueryByMouseDlg : public CDialog
{
private:
	//表示関係++++++++++++++++++++++++++++++++++++++++++++++
	BITMAPINFO bmpInfo; //bmpファイル用
	PAINTSTRUCT ps;

	HWND hwndImage;  //リアルタイム画像出力用領域
	HDC hdcImage;    //デバイスコンテキストのハンドル
	
	int pointx;
	int pointy;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//対話関係++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	int record;   // 対話の履歴
	int nbest;    // NbestのN
	mySentence st[MAX_CAND_NUM];   // 認識結果構造体
	char Text[MY_MAX_ALTERNATES][MAX_ELEMENT_NUM][256];//認識結果単語列
	char Word[MAX_ELEMENT_NUM][256];  // 第1候補の単語列
	int taskflag; // 0は未検出、1は放置物、2は紛失物、3は問い合わせ(放置か紛失かは判断できていない)

	int stateflag; //状態フラグ:0は初期、2は指認識成功(確認をとる)、3は物体認識成功(検索を行う)
	               //           1は認識失敗(途中で初期に戻る)、4はユーザに情報を伝える

	bool endflag;  //検索終了

	char time[3];
	char minute[3];

	int frame;  //物体画像のフレーム数

	int xmin[1],xmax[1],ymin[1],ymax[1];  //指差す領域(検索領域)
	
	Cquery Database;   //検索を行う
	myOutput dummy;      //出力用ダイアログ

	//Multi_Disp SceneOut;  //誤認識時のシーン出力用ダイアログ

	CQuerySlot Qslot,PreSlot;  //問い合わせスロットオブジェクト
	CTalkedContents user,system; //発話内容
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	////再解釈用?+++++++++++++++++++++++++++++++++++++
	//int lnum,leftnum,mnum;
	//int LData[5],LtData[5],MData[5];
	////++++++++++++++++++++++++++++++++++++++++++++++

public:

	//表示関係+++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void ImageOut(unsigned char *pix);   //リアルタイム画像出力
	void Init(); //色々初期化
	void DispSystemmsg(const string &msg);//「システムの発話」にメッセージを表示する関数
	void DispUsermsg(const string &msg);//「ユーザの発話」にメッセージを表示する関数
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//対話制御関連+++++++++++++++++++++++++++++++++++++++++++++++++++
	void SetTask(){taskflag=GessTask();}
	           //0：未検出、1：放置物、2：紛失物
	void ControlDialog(); //対話制御ルーチン
	int GessTask(); //タスクを決定する
	int IntoSlot(CQuerySlot& Q);  //スロットに値を代入
	int SplitResult();//認識結果を認識構造体に分けて入れる
	int SplitWord(char *string, char a, char b, 
			char *string1, 
			char *string2, 
			char *string3); //単語についたタグ(属性値)を取り出す
	void recordInit(){ record = 0; } //対話の履歴をリセット
	void taskflagInit(){taskflag=0;} //タスクフラグをリセット
	void stateflagInit(){stateflag=0;} //状態フラグをリセット
	void GenerateSystemMsg(CQuerySlot &Q); //システムの発話を生成する
	void settask(int n){taskflag=n;}

	// 画像処理関連++++++++++++++++++++++++++++++++++++++++++++++++++
	bool ReferenceObject(); //ユーザが指差している領域を得る
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// コンストラクション
public:
	CQueryByMouseDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_QUERYBYMOUSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOffline();
	afx_msg void OnBnClickedPut();
	afx_msg void OnBnClickedYes();
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedThanks();
	afx_msg void OnBnClickedLeave();
public:
	afx_msg void OnBnClickedOnline();
public:
	afx_msg void OnBnClickedTime();
public:
	afx_msg void OnBnClickedNum();
	int m_xvTime;
public:
	CComboBox m_xcTime;
};
