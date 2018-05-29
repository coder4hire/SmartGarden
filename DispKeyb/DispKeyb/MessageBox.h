#pragma once
#include "ScreenBase.h"
class CMessageBox :
	public CScreenBase
{
public:
	CMessageBox();
	~CMessageBox();

	bool Show(const char* msg, CScreenBase* parent=NULL);
	virtual bool OnKeyPress(int key);
	virtual void Paint();

	static CMessageBox Inst;
	bool GetYesResult() { return isYes; }

protected:
	const char* message;
	bool isYes;
};

