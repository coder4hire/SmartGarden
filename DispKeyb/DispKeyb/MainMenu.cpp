#include "MainMenu.h"
#include "NetworkSettings.h"
#include "MainScreen.h"
#include "stdio.h"

MenuItem CMainMenu::menuItems[MAX_MENU_ITEMS] = {
	{"Main Screen",new CMainScreen()},
	{"Commands",new CNetworkSettings()},
	{"Settings",new CNetworkSettings()}
};

CMainMenu::CMainMenu()
{
	currentPosition = 0;
}

CMainMenu::~CMainMenu()
{
	for (int i=0;i<MAX_MENU_ITEMS;i++)
	{
		delete menuItems[i].ChildScreen;
	}
}

bool CMainMenu::OnKeyPress(int key)
{
	switch(key)
	{
	case KEY_UP:
		currentPosition--;
		if (currentPosition < 0)
		{
			currentPosition = MAX_MENU_ITEMS-1;
		}
		PaintCursor();
		break;
	case KEY_DOWN:
		currentPosition++;
		if (currentPosition == MAX_MENU_ITEMS)
		{
			currentPosition = 0;
		}
		PaintCursor();
		break;
	case KEY_OK:
		menuItems[currentPosition].ChildScreen->Run();
		Paint();
		break;
//	case KEY_CANCEL:
//		return false;
	}
	return true;
}

void CMainMenu::PaintCursor()
{
	for (int i = 0; i < MAX_MENU_ITEMS; i++)
	{
		lcd.GotoXY(0, i);
		lcd.PutChar(i == currentPosition ? '>' : ' ');
	}
}

void CMainMenu::Paint()
{
	printf("Started");
	lcd.Clear();
	for (int i = 0; i<MAX_MENU_ITEMS; i++)
	{
		lcd.GotoXY(2, i);
		lcd.PutS(menuItems[i].Name.c_str());
	}

	PaintCursor();
}

