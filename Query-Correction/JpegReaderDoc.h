// JpegReaderDoc.h : CJpegReaderDoc クラスのインターフェイス
//

//#include "Dib.h"	// ClassView によって追加されました。
#pragma once

class CJpegReaderDoc : public CDocument
{
protected: // シリアル化からのみ作成します。
	CJpegReaderDoc();
	DECLARE_DYNCREATE(CJpegReaderDoc)

// 属性
public:

// 操作
public:

// オーバーライド
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 実装
public:
	virtual ~CJpegReaderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	CDib* GetDib() {return &m_Dib;}			//DIBに変換したJPEGデコード済みデータの取得
	CSize GetPictureSize(void);				// Viewに画像サイズを渡す

protected:
	int		m_iHSize;	// JPEG画像データの幅
	int		m_iVSize;	// JPEG画像データの高さ
	u_char* m_pRedP;	// 赤の要素のビットデータ
	u_char* m_pGreenP;	// 緑の要素のビットデータ
	u_char* m_pBlueP;	// 青の要素のビットデータ
	bool	m_bLoadFlg;	// JPEGファイルロード済みか否かのフラグ
	CDib	m_Dib;		// DIBオブジェクト

	bool LoadJpegFile(LPCTSTR pszFName);	// JPEGファイルを読込む
	bool GetJpegData(u_char*& pRed, u_char*& pGreen, u_char*& pBlue, int & iHSize, int & iVSize);	// JPEGデータの取り出し
	void CreateDIB(void);					// 復号済みのJPEGデータからDIBを作成

};


