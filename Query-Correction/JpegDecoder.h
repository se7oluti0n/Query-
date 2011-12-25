/******************************************************
CJpegDecoder Ver.1.1.0 JPEGデコーダ・クラス
	Copyright 2004 AsWe Co.,Ltd. All rights reserved.
The comments are written in Japanese (Shift-JIS).

JPEGデコーダ・クラス
	JPEGデータをRGB画像データへ変換するクラス

使用方法
	1.インスタンスを生成する
	2.SetJpegData()で画像データへのポインタとサイズを渡す
	3.SetJpegData()の返値でエラーを確認する
	4.DoDisconvert()で逆変換を実行する
	5.DoDisconvert()の返値でエラーを確認する
	6.GetRGBData()でRGB画像データを取り出す

注意事項
	1.このファイルより先に"JPEG.h"をインクルードすること
	2.GetRGBData()の redH, greenH, blueH の領域はCJpegDecoderがnewで確保する。
	　呼び出し側でdeleteすること。
******************************************************/
#include "DecoderTables.h"
#include "JPEG.h"

class CInBitStream;

class CJpegDecoder
{
public:
	// コンストラクタ
				CJpegDecoder( void );
	// デストラクタ
				~CJpegDecoder( void );

	// JPEGデータを設定する
	int		SetJpegData(							// 返値：処理結果
					char*		aJpegDataP,			// JPEGデータ
					int			size );				// JPEGデータのサイズ

	// 復号後の画像データへのポインタを返す。
	//		redH, greenH, blueH の領域はCJpegDecoderがnewで確保する。
	//		呼び出し側でdeleteすること。
	void		GetRGBData(
					u_char**	redH,				// 復号画像へのハンドル
					u_char**	greenH,				// 復号画像へのハンドル
					u_char**	blueH,				// 復号画像へのハンドル
					int&		HSize,				// 横画素数
					int&		VSize );			// 縦画素数

	// 復号実行インタフェース(エラー処理を含む)
	int		Decode( void );							// 返値：復号結果

	// 画像特性の取得
	SJpegProperty	GetProperty( void )				// 返値：JPEG画像の特性
					{	return mProperty; };

protected:
	// 入出力,中間データ
	u_char*				mRgbPA[3];					// 復号画像へのポインタ
	int*				mCompPA[3];					// 画像の各色成分へのポイント(ユニットサイズ)
	int					mPreDCA[3];					// 前ブロックのDC成分(3成分)
	int					mDctDataA[64];				// 1ブロック分のDCTデータ
	int					mBlockA[64];				// 1ブロック分の画素データ

	// 画像情報
	SJpegProperty		mProperty;					// プロパティ
	SScanHeader			mSHeader;					// スキャンヘッダ
	SFrameComponent		mFCompA[3];					// フレーム成分
	SScanComponent		mSCompA[3];					// スキャン成分

	// テーブル
	SHuffmanDecodeTable	mHTA[2][4];					// 成分ハフマンテーブル AC,DC*4種類
	int					mQTA[4][64];				// 量子化テーブル要素 *4成分
	double				mCosT[8][8];				// cosテーブル

	// 復号時状態変数
	int					mEnable;					// 1:SOIからEOIの間を処理中, 0:それ以外
	CInBitStream		*mIBSP;						// JPEGデータ読み出し用ビットストリーム
	char				mHMax;						// 水平最大サンプリングファクタ
	char				mVMax;						// 垂直最大サンプリングファクタ
	int					mRestartInterval;			// リスタートインタバル（インタバル中のMCU数）

	// ヘッダの解析
	int			AnalyzeHeader( void );					// 返値：処理結果
	// マーカ識別
	eMarker		GetMarker( void );					// 返値：マーカコード
	// マーカ解釈
	int			AnalyzeMarker( void );				// 返値：解析結果

	// JPEG復号実行(本処理)
	void		DoDecode( void );
	// MCU復号
	void		DecodeMCU();
	// RGB変換
	void		makeRGB(
					int			ux,					// 横ユニット位置
					int			uy );				// 縦ユニット位置
	// エントロピー符号復号
	void		DecodeHuffmanBlock(
					int			cs );				// 成分番号
	// ハフマン符号1語の復号
	int			Decode1HuffmanCode(
					int			tc,					// テーブルクラス 0:DC成分, 1:AC成分
					int			sc );				// 成分番号
	// 逆量子化
	void		InverseQuantization( 
					int		sc );					// 成分番号
	// 1ブロック逆DCT変換
	void		InverseDCT( void );

	// ハフマンテーブル解析
	void		AnalyzeDHT(
					int			s  );				// サイズ
	// 量子化テーブル解析
	void		AnalyzeDQT(
					int			s  );				// サイズ
	// フレーム解析
	void		AnalyzeFrame( void );
	// スキャンヘッダ解析
	void		AnalyzeScan( void );
	// JFIFフォーマット解析
	void		AnalyzeJFIF( void );
	// JFXXフォーマット解析
	void		AnalyzeJFXX( void );

	// RGB算出用値補正
	u_char		ReviseValue(						// 返値：補正値
					double		v );				// 値
	// エラー時のRGBデータ廃棄
	void		DeleteRGB( void );
	// intサイズの領域確保
	void		NewIntMemory(
					int**		h,					// ハンドル
					int			s,					// サイズ
					int			v,					// 初期値
					char		*mes );				// エラーメッセージ
};

// ブロック数取得
#define GetBlocks(n)	(n>>3)+((n&0x07)>0)
