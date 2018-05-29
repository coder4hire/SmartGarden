#include "MenuBase.h"

CMenuBase::CMenuBase()
{
	currentPosition = 0;
}

CMenuBase::~CMenuBase()
{
	for (int i = 0; i<menuItems.size(); i++)
	{
		delete menuItems[i].ChildScreen;
	}
}

bool CMenuBase::OnKeyPress(int key)
{
	switch (key)
	{
	case KEY_UP:
		currentPosition--;
		if (currentPosition < 0)
		{
			currentPosition = menuItems.size() - 1;
		}
		PaintCursor();
		break;
	case KEY_DOWN:
		currentPosition++;
		if (currentPosition == menuItems.size())
		{
			currentPosition = 0;
		}
		PaintCursor();
		break;
	case KEY_OK:
	{
		bool retVal = OnMenuItemSelected(currentPosition);

		if (menuItems[currentPosition].ChildScreen)
		{
			menuItems[currentPosition].ChildScreen->Run();
		}

		Paint();
		if (!retVal)
		{
			return false;
		}
		break;
	}
	case KEY_CANCEL:
		return false;
	}
	return true;
}

void CMenuBase::PaintCursor()
{
	for (int i = 0; i < menuItems.size(); i++)
	{
		lcd.GotoXY(0, i);
		lcd.PutChar(i == currentPosition ? '>' : ' ');
	}
}

void CMenuBase::Paint()
{
	lcd.Clear();
	for (int i = 0; i<menuItems.size(); i++)
	{
		lcd.GotoXY(2, i);
		lcd.PutS(menuItems[i].Name.c_str());
	}

	PaintCursor();
}