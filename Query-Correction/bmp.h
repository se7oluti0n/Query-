#pragma once

//-----------------
// Header File
//-----------------
#include <iostream>
#include <cstring>
#include "image.h"
using namespace std;

//-----------------
// Class
//-----------------
class bmp : public image {
private:
	/* ファイルヘッダ */
	unsigned short bfType;      // ファイルタイプ
	unsigned long  bfSize;      // ファイルサイズ
	unsigned short bfReserved1; // 予約領域
	unsigned short bfReserved2; // 予約領域
	unsigned long  bfOffset;    // オフセット

	/* 情報ヘッダ */
	unsigned long  biSize;          // 情報ヘッダサイズ
	long  biWidth;                  // 画像の幅
	long  biHeight;                 // 画像の高さ
	unsigned short biPlanes;        // プレーン数
	unsigned short biBitCount;      // 1画素のデータサイズ
	unsigned long  biCompression;   // 圧縮形式
	unsigned long  biSizeImage;     // 画像データサイズ
	long  biXPelsPerMeter;          // 横方向の解像度
	long  biYPelsPerMeter;          // 縦方向の解像度
	unsigned long  biClrUsed;       // 使用色数
	unsigned long  biClrImportant;  // 重要な色の数

public:
	
	// コンストラクタ
	bmp() {};
	// デストラクタ
	~bmp() {};
	
	/* 画像のロード */
	bool load(const string filename);
	/* 画像のセーブ */
	bool save(const string filename);
	bool save2(const string filename);

	bool save_Y(const string filename);
	bool save_Y_d(const string filename);
	bool save_isDiff(const string filename);
};