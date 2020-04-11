#include "MainMenu.h"
#include "NetworkSettings.h"
#include "CommandsMenu.h"
#include "MainScreen.h"
#include "SoundMenu.h"
#include "MessageBox.h"

#include <stdio.h>
#include <stdlib.h>

CMainMenu::CMainMenu()
{
	menuItems.push_back(CMenuItem("Main Screen", new CMainScreen()));
	menuItems.push_back(CMenuItem("Commands", new CCommandsMenu()));
	menuItems.push_back(CMenuItem("Sound", new CSoundMenu()));
	menuItems.push_back(CMenuItem("Settings", new CNetworkSettings()));
}

bool CMainMenu::OnKeyPress(int key)
{
	if (key == (KEY_ABORT | KEY_CANCEL | KEY_OK))
	{
		if (CMessageBox::Inst.Show("Shutdown system ?"))
		{
			system("shutdown 0");
		}
		else
		{
			Paint();
		}
	}
	if (!CMenuBase::OnKeyPress(key))
	{
		// On Esc keypress go to Main Screen
		menuItems[0].ChildScreen->Run();

		// After exitting main screen - repaint menu and go on
		Paint();
	}
	return true;
}

