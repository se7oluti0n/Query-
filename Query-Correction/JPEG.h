/******************************************************
JPEG.h Ver.1.1.0 JPEG関連定数
	Copyright 2004 AsWe.Co.Ltd. All right reseved.
The comments are written in Japanese (Shift-JIS).
******************************************************/

#pragma once

typedef unsigned char	u_char;
typedef unsigned int	u_int;

// 計算、判別用定数
const char		kJFIF[] = "JFIF";				// JFIF識別文字列
const char		kJFXX[] = "JFXX";				// JFXX識別文字列
const double	kSqrt2 = 1.41421356;			// 2の平方根
const double	kDisSqrt2 = 1.0 / kSqrt2;		// 2の平方根の逆数
const double	kPaiDiv16 = 3.14159265 / 16;	// 円周率/16

/************************/
/* ジグザグシーケンス用 */
/************************/

static const int	kZigzagT[64] = {
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

// 処理結果
enum
eProcessResult {
	emNoError			= 0,		// エラー無し
	emRuntimeError		= -1,		// 実行エラー
	emDataError			= -2		// データエラー
};

// マーカコード
enum
eMarker {
// フレーム開始
	// 非差分ハフマン符号
	emSOF0		= 0xc0,		// 基本DCT方式
	emSOF1		= 0xc1,		// 拡張シーケンシャルDCT方式
	emSOF2		= 0xc2,		// プログレッシブDCT方式
	emSOF3		= 0xc3,		// 空間可逆方式
	// 差分ハフマン符号
	emSOF5		= 0xc5,		// 差分シーケンシャルDCT方式
	emSOF6		= 0xc6,		// 差分プログレッシブDCT方式
	emSOF7		= 0xc7,		// 差分空間方式
	// 非差分算術符号
	emJPG		= 0xc8,		// JPEG拡張のためのリザーブ方式
	emSOF9		= 0xc9,		// 拡張シーケンシャルDCT方式
	emSOF10		= 0xca,		// プログレッシブDCT方式
	emSOF11		= 0xcb,		// 空間（シーケンシャル）可逆方式
	// 差分算術符号
	emSOF13		= 0xcd,		// 差分シーケンシャルDCT方式
	emSOF14		= 0xce,		// 差分プログレッシブDCT方式
	emSOF15		= 0xcf,		// 差分空間方式

// ハフマンテーブル指定
	emDHT		= 0xc4,		// ハフマンテーブル定義

// 算術符号条件付け指定
	emDAC		= 0xcc,		// 算術符号化条件付け定義

// リスタートインタバル終了	モジュロ8の0を待つリスタート
	emRST0		= 0xd0,		emRST1		= 0xd1,
	emRST2		= 0xd2,		emRST3		= 0xd3,
	emRST4		= 0xd4,		emRST5		= 0xd5,
	emRST6		= 0xd6,		emRST7		= 0xd7,

// その他
	emSOI		= 0xd8,		// 画像開始
	emEOI		= 0xd9,		// 画像終了
	emSOS		= 0xda,		// スキャン開始
	emDQT		= 0xdb,		// 量子化テーブル定義
	emDNL		= 0xdc,		// ライン数定義
	emDRI		= 0xdd,		// リスタートインタバル定義
	emDHP		= 0xde,		// ハイアラーキカル・プログレッション定義
	emEXP		= 0xdf,		// 参照成分拡大
	emCOM		= 0xfe,		// コメント

// アプリケーションセグメント
	emAPP0		= 0xe0,		emAPP1		= 0xe1,
	emAPP2		= 0xe2,		emAPP3		= 0xe3,
	emAPP4		= 0xe4,		emAPP5		= 0xe5,
	emAPP6		= 0xe6,		emAPP7		= 0xe7,
	emAPP8		= 0xe8,		emAPP9		= 0xe9,
	emAPP10		= 0xea,		emAPP11		= 0xeb,
	emAPP12		= 0xec,		emAPP13		= 0xed,
	emAPP14		= 0xee,		emAPP15		= 0xef,

// JPEG拡張のためのリザーブ
	emJPG0		= 0xf0,		emJPG1		= 0xf1,
	emJPG2		= 0xf2,		emJPG3		= 0xf3,
	emJPG4		= 0xf4,		emJPG5		= 0xf5,
	emJPG6		= 0xf6,		emJPG7		= 0xf7,
	emJPG8		= 0xf8,		emJPG9		= 0xf9,
	emJPG10		= 0xfa,		emJPG11		= 0xfb,
	emJPG12		= 0xfc,		emJPG13		= 0xfd,

// リザーブされたマーカ
	emTEM		= 0x01,		// 算術符号で使用
	emRESst		= 0x02,
	emRESend	= 0xbf,

// エラー処理用非規定マーカ
	emError		= 0xff,		// マーカ検出時エラー発生（検出できなかった）
	emMarker	= 0xff		// マーカ
};

// JPEG画像プロパティ
struct
SJpegProperty {
	int			CanDecode;			// 復号可能性（復号化時のみ）
	int			HSize;				// 元画像横画素数
	int			VSize;				// 元画像縦画素数
	int			Dimension;			// 成分数
	int			SamplePrecision;	// サンプル精度
	char		*CommentP;			// コメント
	int			Format;				// フォーマット種別 0:不明、1:JFIF、2:JFXX 
	u_char		MajorRevisions;		// バージョン小数点上位
	u_char		MinorRevisions;		// バージョン小数点下位
	int			Units;				// 密度単位 0:無し、1:dots/inch、2:dots/cm
	int			HDensity;			// 横密度
	int			VDensity;			// 縦密度
	int			HThumbnail;			// サムネイル横画素数
	int			VThumbnail;			// サムネイル縦画素数
	int			ExtensionCode;		// 拡張コード 0x10:JPEG, 0x11:1byte/pixel, 0x13:3bytes/pixel
};

/********************/
/* エラー通知クラス */
/********************/

// メモリエラー例外通知用クラス
class EMemoryError
{
public:
			EMemoryError(
				int		size,		// 要求サイズ
				char*	messageP )	// エラーメッセージ
				{	mSize=size;	mMessageP=messageP;	};
	
	int			mSize;				// 要求サイズ
	char*		mMessageP;			// エラーメッセージ
};

// テーブル外参照エラー例外通知用クラス
class EIndexError
{
public:
			EIndexError(
				int		maxIndex,	// 許容インデックス
				int		errorIndex, // エラーインデックス
				char*	messageP )		// エラーメッセージ
				{	mMaxIndex=maxIndex;	mErrorIndex=errorIndex;	mMessageP = messageP;	};
	
	int			mMaxIndex;			// 許容インデックス
	int			mErrorIndex;		// エラーインデックス
	char*		mMessageP;			// エラーメッセージ
};

// バッファ終了エラー例外通知用クラス
class	EBufferError
{
public:
			EBufferError(
				char*	messageP )		// エラーメッセージ
				{	mMessageP = messageP;	};
	char*		mMessageP;			// エラーメッセージ
};

// JPEGデータフォーマットエラー例外通知用クラス
class EDataFormatError
{
public:
			EDataFormatError(
				void*	addressP,	// エラー発生アドレス
				char*	messageP )	// エラーメッセージ
				{	mAddressP=addressP;	mMessageP=messageP;	};

	void*		mAddressP;			// エラー発生アドレス
	char*		mMessageP;			// エラーメッセージ
};

// 非対応データエラー通知用クラス
class ENotSupported
{
public:
			ENotSupported(
				eMarker	aMark,		// フレームコード
				void*	addressP )	// エラー発生アドレス
				{ mMark = aMark; mAddressP=addressP; };

	eMarker		mMark;					// フレームコード
	void*		mAddressP;				// エラー発生アドレス
};
