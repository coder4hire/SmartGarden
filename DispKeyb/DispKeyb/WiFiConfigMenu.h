#pragma once
#include "MenuBase.h"

class CWiFiConfigMenu : public CMenuBase
{
public:
	CWiFiConfigMenu();
	virtual ~CWiFiConfigMenu();
	virtual void Paint();
protected:
	virtual bool OnMenuItemSelected(int index);
};

