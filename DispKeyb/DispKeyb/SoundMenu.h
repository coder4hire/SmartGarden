#pragma once
#include "MenuBase.h"

class CSoundMenu : public CMenuBase
{
public:
	CSoundMenu();
	virtual ~CSoundMenu();
	virtual void Paint();
	virtual void OnLeave();
protected:
	virtual bool OnMenuItemSelected(int index);
};

