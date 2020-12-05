#pragma once
#include "MenuBase.h"
class CCommandsMenu :
	public CMenuBase
{
public:
	CCommandsMenu();
	virtual ~CCommandsMenu();
	virtual void Paint();
	virtual bool OnNonPreciseTimer();
protected:
	void PaintStatus();
	virtual bool OnMenuItemSelected(int index);
};

