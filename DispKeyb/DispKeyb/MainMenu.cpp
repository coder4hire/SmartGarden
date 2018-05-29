#include "MainMenu.h"
#include "NetworkSettings.h"
#include "CommandsMenu.h"
#include "MainScreen.h"
#include "stdio.h"

CMainMenu::CMainMenu()
{
	menuItems.push_back(CMenuItem("Main Screen", new CMainScreen()));
	menuItems.push_back(CMenuItem("Commands", new CCommandsMenu()));
	menuItems.push_back(CMenuItem("Settings", new CNetworkSettings()));
}

bool CMainMenu::OnKeyPress(int key)
{
	if (!CMenuBase::OnKeyPress(key))
	{
		// On Esc keypress go to Main Screen
		menuItems[0].ChildScreen->Run();

		// After exitting main screen - repaint menu and go on
		Paint();
	}
	return true;
}

