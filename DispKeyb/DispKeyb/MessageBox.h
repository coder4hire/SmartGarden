#pragma once
#include "ScreenBase.h"
class CMessageBox :
	public CScreenBase
{
	enum State
	{
		OK,
		YES,
		NO
	};
public:
	CMessageBox();
	~CMessageBox();

	bool Show(const char* msg, CScreenBase* parent=NULL, bool askYesNo = true);
	virtual bool OnKeyPress(int key);
	virtual void Paint();

	static CMessageBox Inst;
	bool GetYesResult() { return answer == YES; }

protected:
	const char* message;
	State answer;
};

