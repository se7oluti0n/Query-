//--------------------------------------------------
// ���e�F���b���e��ۑ�����
//
// �쐬�ҁF�q�a�G
// �ύX���F2007/1/26
//--------------------------------------------------
#pragma once

class CTalkedContents
{
private:
	bool area;
	bool object;
	bool time;
	bool result;
	bool scene;  //�ĉ��ߗp

public:
	CTalkedContents(void){};
	~CTalkedContents(void){};

	void InitContents(void);

	//�t���O�𗧂Ă�++++++++++++++++++++++++++++
	void SetArea(){area=true;}
	void SetObject(){object=true;}
	void SetTime(){time=true;}
	void SetResult(){result=true;}
	void SetScene(){scene=true;}
	//++++++++++++++++++++++++++++++++++++++++++

	//�Ԃ�++++++++++++++++++++++++++++++++++++++
	bool GetArea(){return area;}
	bool GetObject(){return object;}
	bool GetTime(){return time;}
	bool GetResult(){return result;}
	bool GetScene(){return scene;}
	//++++++++++++++++++++++++++++++++++++++++++
};