#pragma once

class CFlag
{
private:
	int value;
	bool ok;

public:
	CFlag(){};
	~CFlag(){};

	void InitFlag(){value=0; ok=false;}

	void SetValue(int v){value=v;}
	void SetOk(bool o){ok=o;}

	int GetValue(){return value;}
	bool GetOk(){return ok;}
};