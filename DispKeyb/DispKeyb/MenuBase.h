#pragma once
#include "LcdDisplay.h"
#include "ScreenBase.h"
#include <string>
#include <vector>

struct CMenuItem
{
	std::string Name;
	CScreenBase* ChildScreen;

	CMenuItem(const char *name, CScreenBase* childScreen)
	{
		Name = name;
		ChildScreen = childScreen;
	}
};

class CMenuBase : public CScreenBase
{
public:
	CMenuBase(unsigned int startRow=0);
	virtual ~CMenuBase();
	virtual bool OnKeyPress(int key);

	void PaintCursor();
	virtual void Paint();

protected:
	unsigned int startRow;
	int currentPosition;
	std::vector<CMenuItem> menuItems;

	virtual bool OnMenuItemSelected(int index) { return true; }
};

