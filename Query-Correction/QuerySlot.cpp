//---------------------------------------------------
// ���e�F�₢���킹���s�����߂̃X���b�g�\���̒�`
// 
// �쐬�ҁF�q�a�G
// �X�V���F2007/01/24
//---------------------------------------------------
#include "stdafx.h"
#include "QuerySlot.h"

//-----------------------------------
//name�FInitSlot()
//�X���b�g�̏�����
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