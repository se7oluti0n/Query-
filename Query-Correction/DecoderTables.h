/******************************************************
DecoderTables Ver.1.1.0 JPEG複合クラス用テーブル
	Copyright 2004 AsWe.Co.Ltd. All rights reserved.
The comments are written in Japanese (Shift-JIS).
******************************************************/
#pragma once

enum
eAnalyzedResult {		// ヘッダ解析結果
	emYet			= 0,			// 未解析
	emIsHTable		= 0x01,			// ハフマンテーブル有り
	emIsQTable		= 0x02,			// 量子化テーブル有り
	emIsJFIF		= 0x04,			// JFIFフォーマット指定有り
	emIsComment		= 0x08,			// コメント有り
	emIsStartData	= 0x10			// スキャンデータ有り
};

// フレーム成分指定パラメータ
struct
SFrameComponent {
	u_char		C;					// 成分識別子
	char		H;					// 水平サンプリングファクタ
	char		V;					// 垂直サンプリングファクタ
	char		Tq;					// 量子化テーブルセレクタ
};

// スキャン成分指定パラメータ
struct
SScanComponent {
	u_char		Cs;					// スキャン成分セレクタ
	char		Td;					// DCエントロピ符号化テーブルのセレクタ
	char		Ta;					// ACエントロピ符号化テーブルのセレクタ
};

// スキャンデータのヘッダ
struct
SScanHeader {
	u_char		numOfScanComp;		// スキャン成分数
	u_char		SpectralStart;		// スペクトラル開始
	u_char		SpectralEnd;		// スペクトラル終了
	char		Ah;					// サクセッシブ・アプロキシメーションの上位ビット
	char		Al;					// サクセッシブ・アプロキシメーションの下位ビット
};

// ハフマンテーブル
struct
SHuffmanDecodeTable {
	int			numOfElement;		// テーブル要素数
	int*		SizeTP;				// ハフマンサイズテーブル
	int*		CodeTP;				// ハフマン符号語テーブル
	int*		ValueTP;			// ハフマン符号語に対応した値
};

