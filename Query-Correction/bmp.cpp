//-----------------
// Header File
//-----------------
#include "stdafx.h"
#include <fstream>
#include <string>
#include <cmath>
#include "bmp.h"

long line;  // ラインのデータ長

/****************/
/* 画像のロード */
/****************/
bool bmp::load(const string filename)
{
	ifstream fin;
	long i,j,position;
	
	/* ファイルオープン */
	fin.open(filename.c_str(),ios::binary);
	if(!fin){
		cout << "Cannot open file : " << filename << '\n';
		return false;
	}
	
	/* ヘッダ情報取得 */
	/* ファイルヘッダ情報の取得 */
	fin.read((char *)&bfType,2);      // ファイルタイプ
	fin.read((char *)&bfSize,4);      // ファイルサイズ
	fin.read((char *)&bfReserved1,2); // 予約領域
	fin.read((char *)&bfReserved2,2); // 予約領域
	fin.read((char *)&bfOffset,4);    // オフセット

	/* 情報ヘッダの取得 */
	fin.read((char *)&biSize,4);          // 情報ヘッドサイズ
	fin.read((char *)&biWidth,4);         // 画像の幅
	fin.read((char *)&biHeight,4);        // 画像の高さ
	fin.read((char *)&biPlanes,2);        // プレーン数
	fin.read((char *)&biBitCount,2);      // 1画素のデータサイズ
	fin.read((char *)&biCompression,4);   // 圧縮形式	
	fin.read((char *)&biSizeImage,4);     // 画像データサイズ	
	fin.read((char *)&biXPelsPerMeter,4); // 横方向の解像度	
	fin.read((char *)&biYPelsPerMeter,4); // 縦方向の解像度	
	fin.read((char *)&biClrUsed,4);       // 使用色数	
	fin.read((char *)&biClrImportant,4);  // 重要な色の数
	
	/* 画像データの読み込み */
	/* 画像データの領域を確保 */
	init();
	
	/* 1ラインのデータ長を4byte境界に合わせる */
	line = (biWidth*biBitCount)/8;
	if((line%4)!=0){
		line = ((line/4)+1)*4;
	}
	
	for(i=biHeight-1;i>0;i--){
		/* 行の先頭位置 */
		position=bfOffset+line*i;
		fin.seekg(position,ios::beg);
		
		/* 値の読み込み */
		for(j=0;j<biWidth;j++){
			fin.read((char *)&B(j,i),1);
			fin.read((char *)&G(j,i),1);
			fin.read((char *)&R(j,i),1);
		}
	}
	
	fin.close();  // ファイルクローズ
	
	return true;
}

/****************/
/* 画像のセーブ */
/****************/
bool bmp::save(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* ファイルオープン */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}
	
	/* ファイルヘッダの書き込み */
	fout.write((char *)&bfType,2);       // ファイルタイプ
	fout.write((char *)&bfSize,4);       // ファイルサイズ
	fout.write((char *)&bfReserved1,2);  // 予約領域
	fout.write((char *)&bfReserved2,2);  // 予約領域
	fout.write((char *)&bfOffset,4);     // オフセット
	
	/* 情報ヘッダの書き込み */
	fout.write((char *)&biSize,4);          // 情報ヘッダサイズ
	fout.write((char *)&biWidth,4);         // 画像の幅
	fout.write((char *)&biHeight,4);        // 画像の高さ
	fout.write((char *)&biPlanes,2);        // プレーン数
	fout.write((char *)&biBitCount,2);      // 1画素のデータサイズ
	fout.write((char *)&biCompression,4);   // 圧縮形式
	fout.write((char *)&biSizeImage,4);     // 画像データ
	fout.write((char *)&biXPelsPerMeter,4); // 横方向の解像度
	fout.write((char *)&biYPelsPerMeter,4); // 縦方向の解像度
	fout.write((char *)&biClrUsed,4);       // 使用色数
	fout.write((char *)&biClrImportant,4);  // 重要な色の数
	
	/************************/
	/* 画像データの書き込み */
	/************************/
	/* 行の先頭位置 */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* 値の書き込み */
		for(j=0;j<biWidth;j++){
			fout.write((char *)&B(j,i),1);
			fout.write((char *)&G(j,i),1);
			fout.write((char *)&R(j,i),1);
		}
		/* 1ラインのデータ長を4byte境界に合わせる */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // ファイルクローズ
	
	return true;
}

/****************/
/* 画像のセーブ */
/****************/
bool bmp::save2(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* ファイルオープン */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}
	
	/* ファイルヘッダの書き込み */
	fout.write((char *)&bfType,2);       // ファイルタイプ
	fout.write((char *)&bfSize,4);       // ファイルサイズ
	fout.write((char *)&bfReserved1,2);  // 予約領域
	fout.write((char *)&bfReserved2,2);  // 予約領域
	fout.write((char *)&bfOffset,4);     // オフセット
	
	/* 情報ヘッダの書き込み */
	fout.write((char *)&biSize,4);          // 情報ヘッダサイズ
	fout.write((char *)&biWidth,4);         // 画像の幅
	fout.write((char *)&biHeight,4);        // 画像の高さ
	fout.write((char *)&biPlanes,2);        // プレーン数
	fout.write((char *)&biBitCount,2);      // 1画素のデータサイズ
	fout.write((char *)&biCompression,4);   // 圧縮形式
	fout.write((char *)&biSizeImage,4);     // 画像データ
	fout.write((char *)&biXPelsPerMeter,4); // 横方向の解像度
	fout.write((char *)&biYPelsPerMeter,4); // 縦方向の解像度
	fout.write((char *)&biClrUsed,4);       // 使用色数
	fout.write((char *)&biClrImportant,4);  // 重要な色の数
	
	/************************/
	/* 画像データの書き込み */
	/************************/
	/* 行の先頭位置 */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* 値の書き込み */
		for(j=0;j<biWidth;j++){
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
		}
		/* 1ラインのデータ長を4byte境界に合わせる */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // ファイルクローズ
	
	return true;
}

/****************/
/* 画像のセーブ */
/****************/
bool bmp::save_Y(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* ファイルオープン */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}

	//init file header
	bfType    = 0x4D42;
	bfSize	  = 14+40+line*240;
	bfOffset = 54;

	//init info header
	biSize = 40;
	biWidth = 320;
	biHeight = 240;
	biPlanes = 1;
	biBitCount = 24;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	
	/* ファイルヘッダの書き込み */
	fout.write((char *)&bfType,2);       // ファイルタイプ
	fout.write((char *)&bfSize,4);       // ファイルサイズ
	fout.write((char *)&bfReserved1,2);  // 予約領域
	fout.write((char *)&bfReserved2,2);  // 予約領域
	fout.write((char *)&bfOffset,4);     // オフセット
	
	/* 情報ヘッダの書き込み */
	fout.write((char *)&biSize,4);          // 情報ヘッダサイズ
	fout.write((char *)&biWidth,4);         // 画像の幅
	fout.write((char *)&biHeight,4);        // 画像の高さ
	fout.write((char *)&biPlanes,2);        // プレーン数
	fout.write((char *)&biBitCount,2);      // 1画素のデータサイズ
	fout.write((char *)&biCompression,4);   // 圧縮形式
	fout.write((char *)&biSizeImage,4);     // 画像データ
	fout.write((char *)&biXPelsPerMeter,4); // 横方向の解像度
	fout.write((char *)&biYPelsPerMeter,4); // 縦方向の解像度
	fout.write((char *)&biClrUsed,4);       // 使用色数
	fout.write((char *)&biClrImportant,4);  // 重要な色の数
	
	/************************/
	/* 画像データの書き込み */
	/************************/
	/* 行の先頭位置 */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* 値の書き込み */
		for(j=0;j<biWidth;j++){
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
		}
		/* 1ラインのデータ長を4byte境界に合わせる */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // ファイルクローズ
	
	return true;
}

/****************/
/* 画像のセーブ */
/****************/
bool bmp::save_Y_d(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* ファイルオープン */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}

	//init file header
	bfType    = 0x4D42;
	bfSize	  = 14+40+line*240;
	bfOffset = 54;

	//init info header
	biSize = 40;
	biWidth = 320;
	biHeight = 240;
	biPlanes = 1;
	biBitCount = 24;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	
	/* ファイルヘッダの書き込み */
	fout.write((char *)&bfType,2);       // ファイルタイプ
	fout.write((char *)&bfSize,4);       // ファイルサイズ
	fout.write((char *)&bfReserved1,2);  // 予約領域
	fout.write((char *)&bfReserved2,2);  // 予約領域
	fout.write((char *)&bfOffset,4);     // オフセット
	
	/* 情報ヘッダの書き込み */
	fout.write((char *)&biSize,4);          // 情報ヘッダサイズ
	fout.write((char *)&biWidth,4);         // 画像の幅
	fout.write((char *)&biHeight,4);        // 画像の高さ
	fout.write((char *)&biPlanes,2);        // プレーン数
	fout.write((char *)&biBitCount,2);      // 1画素のデータサイズ
	fout.write((char *)&biCompression,4);   // 圧縮形式
	fout.write((char *)&biSizeImage,4);     // 画像データ
	fout.write((char *)&biXPelsPerMeter,4); // 横方向の解像度
	fout.write((char *)&biYPelsPerMeter,4); // 縦方向の解像度
	fout.write((char *)&biClrUsed,4);       // 使用色数
	fout.write((char *)&biClrImportant,4);  // 重要な色の数
	
	/************************/
	/* 画像データの書き込み */
	/************************/
	/* 行の先頭位置 */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* 値の書き込み */
		for(j=0;j<biWidth;j++){
			Y(j,i)=(unsigned char)Y_d(j,i);
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
			fout.write((char *)&Y(j,i),1);
		}
		/* 1ラインのデータ長を4byte境界に合わせる */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // ファイルクローズ
	
	return true;
}

/****************/
/* 画像のセーブ */
/****************/
bool bmp::save_isDiff(const string filename)
{
	ofstream fout;
	long position,i,j,k;
	
	/* ファイルオープン */
	fout.open(filename.c_str(),ios::binary);
	if(!fout){
		cout << "Cannot open file : " << filename << '\n';
		exit(1);
	}

	//init file header
	bfType    = 0x4D42;
	bfSize	  = 14+40+line*240;
	bfOffset = 54;

	//init info header
	biSize = 40;
	biWidth = 320;
	biHeight = 240;
	biPlanes = 1;
	biBitCount = 24;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	
	/* ファイルヘッダの書き込み */
	fout.write((char *)&bfType,2);       // ファイルタイプ
	fout.write((char *)&bfSize,4);       // ファイルサイズ
	fout.write((char *)&bfReserved1,2);  // 予約領域
	fout.write((char *)&bfReserved2,2);  // 予約領域
	fout.write((char *)&bfOffset,4);     // オフセット
	
	/* 情報ヘッダの書き込み */
	fout.write((char *)&biSize,4);          // 情報ヘッダサイズ
	fout.write((char *)&biWidth,4);         // 画像の幅
	fout.write((char *)&biHeight,4);        // 画像の高さ
	fout.write((char *)&biPlanes,2);        // プレーン数
	fout.write((char *)&biBitCount,2);      // 1画素のデータサイズ
	fout.write((char *)&biCompression,4);   // 圧縮形式
	fout.write((char *)&biSizeImage,4);     // 画像データ
	fout.write((char *)&biXPelsPerMeter,4); // 横方向の解像度
	fout.write((char *)&biYPelsPerMeter,4); // 縦方向の解像度
	fout.write((char *)&biClrUsed,4);       // 使用色数
	fout.write((char *)&biClrImportant,4);  // 重要な色の数
	
	/************************/
	/* 画像データの書き込み */
	/************************/
	/* 行の先頭位置 */
	for(i=0;i<biHeight;i++){
		position=bfOffset+line*i;
		fout.seekp(position,ios::beg);
	
		/* 値の書き込み */
		for(j=0;j<biWidth;j++){
			if(isDiff(j,i)==0)  isDiff(j,i)=0;
			else	isDiff(j,i)=255;
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
			fout.write((char *)&isDiff(j,i),1);
		}
		/* 1ラインのデータ長を4byte境界に合わせる */
		if((biWidth%4)!=0){
			for(k=0;k<((biWidth/4+1)*4)-biWidth;k++){
				unsigned char zero = 0;
				fout.write((char *)&zero,1);
			}
		}
	}
	
	fout.close(); // ファイルクローズ
	
	return true;
}