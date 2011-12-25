//---------------------------------------------------
// 内容：問い合わせを行うためのスロット構造の定義
// 
// 作成者：牧和宏
// 更新日：2007/01/24
//---------------------------------------------------
#include "stdafx.h"
#include "QuerySlot.h"

//-----------------------------------
//name：InitSlot()
//スロットの初期化
//
//----------------------------------
void CQuerySlot::InitSlot()
{
	area_xmin = 0;
	area_xmax = 0;
	area_ymin = 0;
	area_ymax = 0;
	CandObjNum=0;
	time.InitFlag();
	Object.InitFlag();
	area.InitFlag();
	correction.InitFlag();
}