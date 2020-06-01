#pragma once
#include "MenuBase.h"

#define MAX_MENU_ITEMS 3

class CRestartDlg : public CMenuBase
{
public:
	CRestartDlg();
	bool OnMenuItemSelected(int index);
	virtual void Paint();
};
