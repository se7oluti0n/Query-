// constants.h
//
//  配列のサイズなどの定義
///////////////////////////////////////////////////////////////

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define MAX_CAND_NUM 1         //代替解釈数(最大可能代替解釈数とは別)この数でループが回る
                               //10bestまで計算したかったら１０にする
#define MY_MAX_ALTERNATES 30    //最大可能代替解釈数
#define MAX_RECORD_NUM 50      //履歴の最大数
#define MAX_ELEMENT_NUM 20      //最大形態素数
#define MAX_CHAR_NUM_DIC 50     //単語の最大文字数
#define MAX_CATID_NAME 10       //最大カテゴリID部文字数
#define MAX_WORDID_NAME 10      //最大単語ID部文字数
#define MAX_CHAR_NUM_RECO 256   //最大認識文字数

#endif