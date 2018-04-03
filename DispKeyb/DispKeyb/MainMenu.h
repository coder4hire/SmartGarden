#pragma once
#include "LcdDisplay.h"
#include "ScreenBase.h"
#include <string>

#define MAX_MENU_ITEMS 3

struct MenuItem
{
	std::string Name;
	CScreenBase* ChildScreen;
};

class CMainMenu: public CScreenBase
{
public:
	CMainMenu();
	~CMainMenu();
	bool OnKeyPress(int key);

	void PaintCursor();
	virtual void Paint();

protected:
	int currentPosition;
	static MenuItem menuItems[MAX_MENU_ITEMS];
};

