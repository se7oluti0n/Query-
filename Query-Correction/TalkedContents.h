//--------------------------------------------------
// 内容：発話内容を保存する
//
// 作成者：牧和宏
// 変更日：2007/1/26
//--------------------------------------------------
#pragma once

class CTalkedContents
{
private:
	bool area;
	bool object;
	bool time;
	bool result;
	bool scene;  //再解釈用

public:
	CTalkedContents(void){};
	~CTalkedContents(void){};

	void InitContents(void);

	//フラグを立てる++++++++++++++++++++++++++++
	void SetArea(){area=true;}
	void SetObject(){object=true;}
	void SetTime(){time=true;}
	void SetResult(){result=true;}
	void SetScene(){scene=true;}
	//++++++++++++++++++++++++++++++++++++++++++

	//返す++++++++++++++++++++++++++++++++++++++
	bool GetArea(){return area;}
	bool GetObject(){return object;}
	bool GetTime(){return time;}
	bool GetResult(){return result;}
	bool GetScene(){return scene;}
	//++++++++++++++++++++++++++++++++++++++++++
};