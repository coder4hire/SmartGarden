#pragma once
#include "MenuBase.h"

#define MAX_MENU_ITEMS 3

class CMainMenu: public CMenuBase
{
public:
	CMainMenu();
	virtual bool OnKeyPress(int key);
};

