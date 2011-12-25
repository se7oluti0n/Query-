/******************************************************
CJpegDecoder JPEGデコーダ・クラス Ver.1.1.0
	Copyright 2004 AsWe Co.,Ltd.
	All rights reserved.
The comments are written in Japanese (Shift-JIS).
******************************************************/
#include "stdafx.h"

#include <math.h>		// VC++でも必要
#include <string.h>	    // 標準C++の場合
#include "JPEG.h"
#include "InBitStream.h"
#include "JpegDecoder.h"

// コンストラクタ
CJpegDecoder::CJpegDecoder( void )
{
	// ハフマンテーブル初期化
	for( int i=0; i<2; i++ )		// 0:DC成分, 1:AC成分
		for( int j=0; j<4; j++ ){	// 4成分
			mHTA[i][j].SizeTP = mHTA[i][j].CodeTP = mHTA[i][j].ValueTP = NULL;
			mHTA[i][j].numOfElement = 0;
		}

	// 画像属性初期化
	mProperty.CanDecode = emYet;	// 未解析
	mProperty.Format = 0;			// フォーマット指定無し
	mProperty.MajorRevisions = 0;	// バージョン指定無し
	mProperty.MinorRevisions = 0;	// バージョン指定無し
	mProperty.Units = 0;			// 単位無し
	mProperty.HDensity = 1;			// 横密度1
	mProperty.VDensity = 1;			// 縦密度1
	mProperty.HThumbnail = 0;		// サムネイル横画素数0
	mProperty.VThumbnail = 0;		// サムネイル縦画素数0
	mProperty.ExtensionCode = 0;	// 拡張コード無し
	mProperty.SamplePrecision = 0;	// サンプル精度8ビット
	mProperty.CommentP= NULL;		// コメント無し
	mRestartInterval = 0;			// リスタートインタバル初期化：無効
	mIBSP = NULL;					// ビットストリーム無し

	// cosテーブル作成
	for( int n=0; n<8; n++ )
		for( int m=0; m<8; m++ )
			mCosT[n][m] = cos(( 2 * m + 1) * n * kPaiDiv16);
}

// デストラクタ
CJpegDecoder::~CJpegDecoder( void )
{
	// テーブル廃棄
	for( int i=0; i<2; i++ )		// 0:DC成分, 1:AC成分
		for( int j=0; j<4; j++ ){	// 4成分
			if( mHTA[i][j].CodeTP != NULL )
				delete mHTA[i][j].CodeTP;
			if( mHTA[i][j].SizeTP != NULL )
				delete mHTA[i][j].SizeTP;
			if( mHTA[i][j].ValueTP != NULL )
				delete mHTA[i][j].ValueTP;
		}

	// コメント領域廃棄
	if( mProperty.CommentP != NULL )
		delete mProperty.CommentP;

	// ビットストリームの廃棄
	if( mIBSP != NULL )
		delete mIBSP;
}

// JPEGデータを設定する
int
CJpegDecoder::SetJpegData(
	char*		aJpegDataP,			// JPEGデータ
	int			size )				// JPEGデータのサイズ
{
	// ビットストリームの生成
	if( mIBSP != NULL )
		delete mIBSP;
	mIBSP = new CInBitStream( aJpegDataP, size );
	// ヘッダの解析
	return	AnalyzeHeader();			// ヘッダ解析結果を返す
}

// 変換後の画像データへのポインタを返す。
void
CJpegDecoder::GetRGBData(
	u_char**	redH,				// 復号画像へのポインタ
	u_char**	greenH,				// 復号画像へのポインタ
	u_char**	blueH,				// 復号画像へのポインタ
	int&		HSize,				// 横画素数
	int&		VSize )				// 縦画素数
{
	HSize = mProperty.HSize;
	VSize = mProperty.VSize;

	*redH = mRgbPA[0];
	*greenH = mRgbPA[1];
	*blueH = mRgbPA[2];
}

#ifdef _MSC_VER
#pragma warning(disable : 4101)
#endif

// 逆変換実行
int
CJpegDecoder::Decode( void )
{
	try{
		// ハフマンテーブル,量子化テーブル,スキャンデータの存在の確認
		if( !(mProperty.CanDecode & (emIsHTable|emIsQTable|emIsStartData)) )
			return	emDataError;

		// 復号可能ならば
		DoDecode();
		return	emNoError;
	}

	catch( EMemoryError &me ){							// メモリエラー発生
		DeleteRGB();									// RGB領域の廃棄
		return	emRuntimeError;							// 実行時エラー
	}

	catch( EIndexError &ie ){							// テーブルアクセス違反
		DeleteRGB();									// RGB領域の廃棄
		return	emRuntimeError;							// 実行時エラー
	}

	catch( EBufferError &eob ){							// バッファアクセス違反
		DeleteRGB();									// RGB領域の廃棄
		return	emRuntimeError;							// 実行時エラー
	}

	catch( EDataFormatError &dfe ){						// ハフマン符号復号失敗
		DeleteRGB();									// RGB領域の廃棄
		return	emDataError;							// データエラー
	}

	catch( ENotSupported &ns ){							// 非対応フォーマット
		DeleteRGB();									// RGB領域の廃棄
		return	emDataError;							// データエラー
	}
}

/******************************* protected ***********************************/
// ヘッダの解析

int
CJpegDecoder::AnalyzeHeader( void )
{
	mEnable = 0;
	eMarker		mark;

	try{
		// 画像開始マーカを探す
		while( mEnable == 0 ){							// SOIマーカを探す
			mark = GetMarker();
			if( mark == emSOI )							// 画像開始
				mEnable = 1;
		}

		// 解析処理
		while( mEnable == 1 ){							// EOIマーカまで
			mProperty.CanDecode |= AnalyzeMarker();		// 解析実行＆結果の更新
			if( mProperty.CanDecode & emIsStartData )	// スキャンデータの始まり
				return	(int)emNoError;
		}

		return (int)emNoError;							// エラー無し（スキャンデータ無し）
	}	// end try

	// エラー処理
	catch( EMemoryError &me ){							// メモリエラー発生
		return (int)emRuntimeError;						// 実行時エラー
	}

	catch( EIndexError &ie ){							// テーブルアクセス違反
		return (int)emDataError;						// データ不正によるアクセス違反
	}

	catch( EBufferError &eob ){							// バッファアクセス違反
		return (int)emDataError;						// EOIマーカが含まれていない
	}

	catch( EDataFormatError &dfe ){						// ハフマン符号の復号失敗
		return (int)emDataError;						// データエラー
	}

	catch( ENotSupported &ns ){							// 非対応フォーマット
		return (int)emDataError;						// データ不正によるアクセス違反
	}
}

#ifdef _MSC_VER
#pragma warning(default : 4101)
#endif

// マーカ解釈
int
CJpegDecoder::AnalyzeMarker( void )					// 返値：解析結果
{
	int		length;
	eMarker	mark = GetMarker();

	switch( mark ){
		case emSOF0:	// フレーム処理(非差分ハフマン符号：基本DCT方式)
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			AnalyzeFrame();
			break;

		case emDHT:		// ハフマンテーブル
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			AnalyzeDHT( length );
			return emIsHTable;							// ハフマンテーブル有り

		case emDNL:		// ライン数定義
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			mProperty.VSize = mIBSP->GetWord();			// ライン数
			break;

		case emDQT:		// 量子化テーブル定義
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			AnalyzeDQT( length );
			return emIsQTable;							// 量子化テーブル有り

		case emEOI:		// 画像終了
			mEnable = 0;
			break;

		case emSOS:		// スキャン開始
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			AnalyzeScan();
			return emIsStartData;						// スキャンデータ有り

		case emDRI:		// リスタートインタバル定義
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			mRestartInterval = mIBSP->GetWord();
			break;

		case emCOM:		// コメント
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			if( mProperty.CommentP != NULL )				// 最後のコメントのみ有効とする
				delete mProperty.CommentP;
			mProperty.CommentP = new( char[length + 1] );	// 念のため1バイト余計にとって'\0'をつける
			mIBSP->CopyByte( mProperty.CommentP, length );
			*(mProperty.CommentP+length) = '\0';			// 念のため
			return emIsComment;

		// 非対応フレーム処理
		case emSOF1:	// 非差分ハフマン符号：拡張シーケンシャルDCT方式
		case emSOF2:	// 非差分ハフマン符号：プログレッシブDCT方式
		case emSOF3:	// 非差分ハフマン符号：空間可逆方式
		case emSOF5:	// 差分ハフマン符号：差分シーケンシャルDCT方式
		case emSOF6:	// 差分ハフマン符号：差分プログレッシブDCT方式
		case emSOF7:	// 差分ハフマン符号：差分空間方式
		case emSOF9:	// 非差分算術符号：拡張シーケンシャルDCT方式
		case emSOF10:	// 非差分算術符号：プログレッシブDCT方式
		case emSOF11:	// 非差分算術符号：空間（シーケンシャル）可逆方式
		case emSOF13:	// 差分算術符号：差分シーケンシャルDCT方式
		case emSOF14:	// 差分算術符号：差分プログレッシブDCT方式
		case emSOF15:	// 差分算術符号：差分空間方式
		case emEXP:		// 参照成分拡大
		case emDAC:		// 算術符号化条件付け定義
		case emDHP:		// ハイアラーキカル・プログレッション定義
			throw( ENotSupported( mark, mIBSP->GetNextAddress() ) );
			break;

		// アプリケーションデータセグメント
		case emAPP0:	// JPEG FIF
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			if( length >= 5 ){
				char	id[5];
				mIBSP->CopyByte( id, 5 );
				id[4] = '\0';							// 念のため
				if( strcmp( kJFIF, id ) == 0 ){			// JFIFフォーマットならば
					AnalyzeJFIF();
					mIBSP->SkipByte( length - 14 );		// サムネイルはサポートしない
				}
				else if( strcmp( kJFXX, id ) == 0 ){	// JFXXフォーマットならば
					AnalyzeJFXX();
					mIBSP->SkipByte( length - 1 );		// サムネイルはサポートしない
				}
				else									// JFIFでもJFXXでもないならば
					mIBSP->SkipByte( length - 5 );		// このバージョンではサポートしないので読み飛ばす
			}
			else
				mIBSP->SkipByte( length );
			break;

		// 非対応マーカ
		case emAPP1:	case emAPP2:	case emAPP3:	case emAPP4:
		case emAPP5:	case emAPP6:	case emAPP7:	case emAPP8:
		case emAPP9:	case emAPP10:	case emAPP11:	case emAPP12:
		case emAPP13:	case emAPP14:	case emAPP15:
			length = mIBSP->GetWord() - 2;				// パラメータ長指定部分を引く
			mIBSP->SkipByte( length );					// このバージョンではサポートしないので読み飛ばす
			break;

		// リザーブ他
		case emJPG:		case emJPG0:	case emJPG1:	case emJPG2:
		case emJPG3:	case emJPG4:	case emJPG5:	case emJPG6:
		case emJPG7:	case emJPG8:	case emJPG9:	case emJPG10:
		case emJPG11:	case emJPG12:	case emJPG13:	case emTEM:
		// エラー
		case emError:
		default:
			throw( EDataFormatError( mIBSP->GetNextAddress()-1, "Marker error" ));
			break;
	}
	
	return	emYet;										// 解析継続
}

// マーカ識別
// 読み出し位置をマーカの次にする
eMarker
CJpegDecoder::GetMarker( void )
{
	while( 1 ){	// バッファの終わりまで繰り返す バッファが終わるとCInBitStreamでthrowする
		u_char	c = mIBSP->GetByte();
		if( c == (u_char)emMarker ){					// 0xffのとき
			c = mIBSP->GetByte();						// 次のバイトを取り出し
			if( c != (u_char)0 )						// 0でなければ
				if( c > (u_char)emRESst && c < (u_char)emSOF0 )	// 定義されたマーカでないなら
					return	emError;					// エラー
				else									// 定義されたマーカなら
					return (eMarker)c;					// マーカを返す
		}
	}
}

// ハフマンテーブル解析
void
CJpegDecoder::AnalyzeDHT(
	int			s )			// サイズ
{
	// 終了アドレス
	char*	theEndAddressP = mIBSP->GetNextAddress();
	if( theEndAddressP == NULL )
		throw( EDataFormatError( theEndAddressP, "AnalyzeDHT:DHT Length" ) );
	theEndAddressP += s;

	// 解析開始
	do{
		u_char	uc = mIBSP->GetByte();
		int		tc, th;
		if( (tc = uc>>4) > 1 )							// 0:DC成分, 1:AC成分
			throw( EDataFormatError( mIBSP->GetNextAddress()-1, "AnalyzeDHT:Tc" ) );
		if( (th = uc & 0x0f) > 3 )						// 0～3
			throw( EDataFormatError( mIBSP->GetNextAddress()-1, "AnalyzeDHT:Th" ) );
		SHuffmanDecodeTable 	&aHT = mHTA[tc][th];		// 格納領域を指定

		int		i;										// カウンタ
		int		num = 0;								// 符号語の数
		u_char	cc[16];
		for( i=0; i<16; i++ ){
			cc[i] = mIBSP->GetByte();
			num += cc[i];
		}
		aHT.numOfElement = num;							// 要素数

		// テーブル領域確保
		NewIntMemory( &aHT.SizeTP, num, 0, "AnalyzeDHT:Huffman Size-T" );
		NewIntMemory( &aHT.CodeTP, num, 0, "AnalyzeDHT:Huffman Code-T" );
		NewIntMemory( &aHT.ValueTP, num, 0, "AnalyzeDHT:Huffman Value-T" );

		// サイズテーブル生成
		int		k = 0;									// インデックス
		for( i=1; i<=16; i++ ){							// 符号語長iビットについて
			int	j=1;									// SizeTP[]に記録した数が
			while( j <= cc[i-1] ){						// iビットの符号語数まで
				if( k >= num )							// 符号語総数がオーバーしたら
					throw( EIndexError( num, k, "AnalyzeDHT:Size Table Index over" ) );	// アクセス違反監視
				aHT.SizeTP[k++] = i;					// 符号語長（ビット数）を記録
				j++;
			}
		}

		// 符号語テーブル生成
		k = 0;											// インデックス
		int	code = 0;									// 符号語（コード）
		int	si = aHT.SizeTP[0];							// サイズ（ビット数）
		while( 1 ){
			while( aHT.SizeTP[k] == si )				// サイズの確認
				aHT.CodeTP[k++] = code++;				// 符号語の決定
			if( k >= num )								// 終了判定
				break;									// 終了

			do {										// 次のサイズの準備
				code <<= 1;								// 符号語長を1ビット増やす
				si++;									// サイズを1ビット増やす
			} while( aHT.SizeTP[k] != si );				// siビットの符号語がないとき繰り返し
		}	// while(1)

		// 復号テーブル生成
		for( k=0; k<num; k++ )
			aHT.ValueTP[k] = mIBSP->GetByte();			// 対応する値の取り出し

	}while( mIBSP->GetNextAddress() < theEndAddressP );
}

// 量子化テーブル解析
void
CJpegDecoder::AnalyzeDQT(
	int			s )		// サイズ
{
	char* theEndAddressP = mIBSP->GetNextAddress() + s;	// 終了アドレス

	do {
		// テーブルの指定
		u_char	c = mIBSP->GetByte();
		int		*aQT = mQTA[ c&0x3 ];					// テーブルは最大4つ

		int		Pq = c >> 4;							// エレメント精度 0:要素Qkは8ビット, 1:16ビット
		if( Pq == 0 )									// 8ビット要素
			for( int i=0; i<64; i++ )
				aQT[kZigzagT[i]] = int( u_char( mIBSP->GetByte() ) );
		else											// 16ビット要素
			for( int i=0; i<64; i++ )
				aQT[kZigzagT[i]] = mIBSP->GetWord();

	}while( mIBSP->GetNextAddress() < theEndAddressP );
}

// フレームヘッダ解析
void
CJpegDecoder::AnalyzeFrame( void )
{
	mProperty.SamplePrecision = mIBSP->GetByte();		// サンプル精度
	mProperty.VSize = mIBSP->GetWord();					// 画像横サイズ
	mProperty.HSize = mIBSP->GetWord();					// 画像縦サイズ
	mProperty.Dimension = mIBSP->GetByte();				// 成分数
	if( mProperty.Dimension != 3 && mProperty.Dimension != 1 )	// 成分数が3または1でない場合
		throw( ENotSupported( emSOF0, mIBSP->GetNextAddress() ) );	// サポートしない

	int i;
	mHMax = mVMax = 0;
	for( i=0; i<mProperty.Dimension; i++ ){
		mFCompA[i].C = mIBSP->GetByte();					// 成分識別子
		u_char	c = mIBSP->GetByte();

		mFCompA[i].H = c >> 4;							// 水平サンプリングファクタ
		if( mFCompA[i].H > mHMax )
			mHMax = mFCompA[i].H;

		mFCompA[i].V = c & 0xf;							// 垂直サンプリングファクタ
		if( mFCompA[i].V > mVMax )
			mVMax = mFCompA[i].V;

		mFCompA[i].Tq = mIBSP->GetByte();				// 量子化テーブルセレクタ
	}
}

// スキャンヘッダ解析
void
CJpegDecoder::AnalyzeScan( void )
{
	u_char	c;
	mSHeader.numOfScanComp = mIBSP->GetByte();			// スキャン成分数

	for( int i=0; i<mSHeader.numOfScanComp; i++ ){
		mSCompA[i].Cs = mIBSP->GetByte();				// スキャン成分セレクタ
		c = mIBSP->GetByte();
		mSCompA[i].Td = c >> 4;							// DC成分用ハフマンテーブルのセレクタ
		if( mSCompA[i].Td > 2 )
			throw( EIndexError( 2, mSCompA[i].Td, "AnalyzeScan:Td Over" ) );
		mSCompA[i].Ta = c & 0xf;							// AC成分用ハフマンテーブルのセレクタ
		if( mSCompA[i].Ta > 2 )
			throw( EIndexError( 2, mSCompA[i].Ta, "AnalyzeScan:Ta Over" ) );
	}

	mSHeader.SpectralStart = mIBSP->GetByte();			// 未使用
	mSHeader.SpectralEnd = mIBSP->GetByte();			// 未使用
	c = mIBSP->GetByte();								// 未使用
	mSHeader.Ah = c >> 4;								// 未使用
	mSHeader.Al = c & 0xf;								// 未使用
}

// JFIFフォーマット解析
void
CJpegDecoder::AnalyzeJFIF( void )
{
	mProperty.Format = 1;								// JFIF
	mProperty.MajorRevisions = mIBSP->GetByte();		// バージョン上位
	mProperty.MinorRevisions = mIBSP->GetByte();		// バージョン下位
	mProperty.Units = mIBSP->GetByte();					// 単位 0:無し、1:dots/inch、2:dots/cm
	mProperty.HDensity = mIBSP->GetWord();				// 横密度
	mProperty.VDensity = mIBSP->GetWord();				// 縦密度
	mProperty.HThumbnail = mIBSP->GetByte();			// サムネイル横画素数
	mProperty.VThumbnail = mIBSP->GetByte();			// サムネイル縦画素数
	mProperty.CanDecode |= emIsJFIF;					// JFIFフォーマット指定有り
}

// JFXXフォーマット解析
void
CJpegDecoder::AnalyzeJFXX( void )
{
	mProperty.Format = 2;								// JFXX
	mProperty.ExtensionCode = mIBSP->GetByte();			// 拡張コード 0x10:JPEG, 0x11:1byte/pixel, 0x13:3bytes/pixel
}

// JPEG逆変換実行
void
CJpegDecoder::DoDecode( void )
{
	// 定数の計算
	int		VBlocks = GetBlocks( mProperty.VSize );		// 縦ブロック数
	int		HBlocks = GetBlocks( mProperty.HSize );		// 横ブロック数
	int		numHUnit = HBlocks / mHMax;					// 横ユニット数
	if( HBlocks % mHMax )
		numHUnit++;
	int		numVUnit = VBlocks / mVMax;					// 縦ユニット数
	if( VBlocks % mVMax )
		numVUnit++;

	// 結果の格納領域を確保
	int		rgbSize = (numVUnit * mVMax) * 8 * (numHUnit * mHMax) * 8;	// 符号に含まれる付加部分も含んでいる

	mRgbPA[0] = new u_char[ rgbSize ];					// 赤成分
	if( mRgbPA[0] == NULL )
		throw( EMemoryError( rgbSize, "DoDecode:mRgbPA[0]" ));
	mRgbPA[1] = new u_char[ rgbSize ];					// 緑成分
	if( mRgbPA[1] == NULL )
		throw( EMemoryError( rgbSize, "DoDecode:mRgbPA[1]" ));
	mRgbPA[2] = new u_char[ rgbSize ];					// 青成分
	if( mRgbPA[2] == NULL )
		throw( EMemoryError( rgbSize, "DoDecode:mRgbPA[2]" ));

	// 結果の格納領域の確保
	mCompPA[0] = mCompPA[1] = mCompPA[2] = NULL;
	int	unitSize = mHMax * mVMax * 64;					// ユニット内データサイズ
	NewIntMemory( &mCompPA[0], unitSize, 0, "Disconvert unit1" );
	NewIntMemory( &mCompPA[1], unitSize, 0x80, "Disconvert unit2" );
	NewIntMemory( &mCompPA[2], unitSize, 0x80, "Disconvert unit3" );

	mPreDCA[0] = mPreDCA[1] = mPreDCA[2] = 0;			// 前ブロックのDC成分値を初期化
	int		restartCount = 0;							// インタバルカウンタ

	// ユニットごとに処理
	for( int uy=0; uy<numVUnit; uy++ ){					// 縦ユニット指数
		for( int ux=0; ux<numHUnit; ux++ ){				// 横ユニット指数

			DecodeMCU();								// MCUの処理
			makeRGB( ux, uy );							// RGBに変換

			if( mRestartInterval ){						// リスタートインタバル有効ならば
				if(++restartCount >= mRestartInterval){	// インタバル終了ならば
					restartCount = 0;					// カウンタリセット
					eMarker		mark = GetMarker();		// マーカ検出
					if( mark >= emRST0 && mark <= emRST7 )	// リスタートマーカならば
						mPreDCA[0] = mPreDCA[1] = mPreDCA[2] = 0;	// 前ブロックのDC成分値をリセット
				}	// if( ++restartCount >= ･･･
			}	// if( mRestartInterval )
		}	// for ux
	}	// for uy

	delete mCompPA[0];	delete mCompPA[1];	delete mCompPA[2];
}

// MCUデコード
void
CJpegDecoder::DecodeMCU( void )
{
	// 成分ごとに
	for( int sc=0; sc<mProperty.Dimension; sc++ ){
		int	numV = mFCompA[sc].V;						// 垂直サンプリングファクタ
		int	numH = mFCompA[sc].H;						// 水平サンプリングファクタ
		int	cntY = mVMax/numV;							// ユニット内重複数
		int	cntX = mHMax/numH;							// ユニット内重複数
		int	vStep = mHMax * 8;							// 縦方向のアドレス増加量

		for( int ky=0; ky<numV; ky++ ){					// ユニット内縦ブロックごと
			for( int kx=0; kx<numH; kx++ ){				// ユニット内横ブロックごと
				DecodeHuffmanBlock( sc );				// エントロピー符号の復号
				InverseQuantization( sc );				// 逆量子化
				InverseDCT();							// 逆DCT変換

				// ブロック単位のデータを表示順序に変換＆間引かれたものを元に戻す
				int*	tp = mCompPA[sc] + ky * vStep * 8 + kx * 8;
				for( int yInU=0; yInU<8*cntY; yInU++ )
					for( int xInU=0; xInU<8*cntX; xInU++ )
						tp[ yInU * vStep + xInU ] = mBlockA[(yInU / cntY) * 8 + (xInU / cntX)];
			}	// kx
		}	// ky
	}	// sc
}

// RGB変換
void
CJpegDecoder::makeRGB(
	int			ux,										// 横ユニット位置
	int			uy )									// 縦ユニット位置
{
	int*	yp = mCompPA[0];							// Y
	int*	up = mCompPA[1];							// Cb
	int*	vp = mCompPA[2];							// Cr
	int		numLine = uy * mVMax * 8;					// ライン：縦ユニット数×ユニットライン数
	int		offsetV = numLine * mProperty.HSize;		// 縦オフセット：ライン×１ラインドット数
	int		offsetH = ux * mHMax * 8;					// 横オフセット：横ユニット数×ユニット内横ブロック数×ブロック内ドット数
	int		offset = offsetV + offsetH;					// 総合オフセット
	u_char*	rp = mRgbPA[0] + offset;					// 赤書き込み位置
	u_char*	gp = mRgbPA[1] + offset;					// 緑書き込み位置
	u_char*	bp = mRgbPA[2] + offset;					// 青書き込み位置
	int		endX = mHMax*8;								// 横ループ終了ポイント
	int		endY = mVMax*8;								// 縦ループ終了ポイント

	for( int picY=0; picY<endY; picY++ ){
		for( int picX=0; picX<endX; picX++ ){
			if( picX + offsetH >= mProperty.HSize ){	// 付加ビットならば
				yp += endX - picX;						// ソースアドレスを更新
				up += endX - picX;
				vp += endX - picX;
				break;									// 次のラインへ
			}

			int	index = picY * mProperty.HSize + picX;	// 書き込み位置

#ifdef GRAY
			gp[index] = bp[index] =	rp[index] = ReviseValue( *yp++ );
#else
	double	v = *yp							+ (*vp - 0x80) * 1.4020;
			rp[index] = ReviseValue( v );				// R
			v = *yp - (*up - 0x80) * 0.3441	- (*vp - 0x80) * 0.7139;
			gp[index] = ReviseValue( v );				// G
			v = *yp + (*up - 0x80) * 1.7718;
			bp[index] = ReviseValue( v );				// B

			yp++; vp++; up++;
#endif
		}	// picX
	}	// picY
}

// 1ブロック分のエントロピー符号を復号（逆量子化を含む）
void
CJpegDecoder::DecodeHuffmanBlock(
	int		sc )										// 成分番号
{
// DC成分復号
	int		diff = 0;									// DC成分差分
	// ハフマン符号1語の復号
	int category = Decode1HuffmanCode( 0, sc );			// カテゴリ（差分値ビット数）
	if( category > 0 ){
		diff = mIBSP->GetBits( category );
		if( ( diff & ( 1 << (category-1) )) == 0 )		// 差分が負
			diff -= (1 << category ) - 1;
	}
	else if( category < 0 )								// マーカ
		throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:Error Marker" ) );

	mPreDCA[sc] += diff;
	mDctDataA[0] = mPreDCA[sc];							// DC成分値設定（並び替え不要）

// AC成分復号

	int 	k = 1;
	while( k < 64 ){
		// ハフマン符号１語の復号
		category = Decode1HuffmanCode( 1, sc );			// ランレングスとカテゴリ
		if( category == 0 ){							// EOB
			while( k < 64 )
				mDctDataA[kZigzagT[k++]] = 0;
			break;
		}
		else if( category < 0 ) 						// マーカ
			throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:Error Marker" ) );

		int	run = category >> 4;						// ランレングス
		category &= 0x0f;								// カテゴリ

		int	acv = 0;									// AC成分値
		if( category ){
			acv = mIBSP->GetBits( category );			// AC成分取り出し
			if( (acv & ( 1 << (category-1) )) == 0 )	// 成分が負
				acv -= (1 << category ) - 1;
		}
		else if( run != 15 )							// EOBでもZRLでない
			throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:wrong Huffman value" ) );

		if( run + k > 63 )								// 係数が多すぎる
			throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:wrong Run length" ) );

		while( run-- > 0 )								// ランレングスの数だけ0
			mDctDataA[kZigzagT[k++]] = 0;

		mDctDataA[kZigzagT[k++]] = acv;					// 順序変換

	}
}

// ハフマン符号1語の復号
int
CJpegDecoder::Decode1HuffmanCode(
	int		tc,											// テーブルクラス 0:DC成分, 1:AC成分
	int		sc )										// 成分番号
{
	// ハフマンテーブル指定
	SHuffmanDecodeTable	&theHT = mHTA[tc][mSCompA[sc].Td];	// 使用するハフマンテーブル

	int		code = 0;									// ハフマン符号語の候補：最大値16ビット
	int		length = 0;									// ハフマン符号語候補のビット数
	int		next = 0;									// 次の1ビット
	int		k = 0;										// 表の指数

	while( k < theHT.numOfElement && length < 16 ){
		length++;
		code <<= 1;
		next = mIBSP->GetBits( 1 );
		if( next < 0 )									// マーカだったら
			return	next;
		code |= next;

		while( theHT.SizeTP[k] == length ){				// 候補と符号語のビット数が等しい間だ検索
			if( theHT.CodeTP[k] == code )				// ヒット
				return theHT.ValueTP[k];				// 復号結果を返す
			k++;										// 次の符号語
		}
	}

	throw( EDataFormatError( mIBSP->GetNextAddress(), "Decode1HuffmanCode:length over" ) );
}

// 逆量子化
void
CJpegDecoder::InverseQuantization( 
	int		sc )											// 成分番号
{
	int	*p = mDctDataA;										// DCT係数データ
	int	*aQT = mQTA[mFCompA[sc].Tq];							// 量子化テーブル

	for( int i=0; i<64; i++ )
		*p++ *= *aQT++;										// 逆量子化
}

// 逆DCT変換
void
CJpegDecoder::InverseDCT( void )
{
	int	sl = mProperty.SamplePrecision == 8 ? 128 : 2048;	// レベルシフト値の決定

	for( int y=0; y<8; y++ ){
		for( int x=0; x<8; x++ ){
			double	sum = 0;

			for( int v=0; v<8; v++ ){
				double cv = v==0 ? kDisSqrt2 : 1.0;
				for( int u=0; u<8; u++ ){
					double cu = u==0 ? kDisSqrt2 : 1.0;

					sum += cu * cv * mDctDataA[ v * 8 + u ] * mCosT[u][x] * mCosT[v][y];
				}
			}
			mBlockA[ y*8+x ] = int( sum / 4 + sl );
		}
	}
}


// RGB算出用値補正
u_char
CJpegDecoder::ReviseValue(								// 返値：補正値
	double	v )											// 値
{
	if( v < 0.0 )
		return 0;
	if( v > 255.0 )
		return 255;
	return (u_char)v;
}

// エラー時のRGBデータ領域の廃棄
void
CJpegDecoder::DeleteRGB( void )
{
	if( mRgbPA[0] != NULL )	delete mRgbPA[0];
	if( mRgbPA[1] != NULL )	delete mRgbPA[1];
	if( mRgbPA[2] != NULL )	delete mRgbPA[2];
}

// intサイズの領域確保
void
CJpegDecoder::NewIntMemory(
	int**	h,			// ハンドル
	int		s,			// サイズ
	int		v,			// 初期値
	char*	mes )		// エラーメッセージ
{
	if( *h != NULL )
		delete *h;
	*h = new int[s];
	if( *h == NULL )
		throw( EMemoryError( s, mes ) );
	while( s-- )
		(*h)[s] = v;
}