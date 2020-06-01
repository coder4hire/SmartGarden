#include "RestartDlg.h"
#include "stdlib.h"

CRestartDlg::CRestartDlg() :CMenuBase(1)
{
	menuItems.push_back(CMenuItem("Reboot", NULL));
	menuItems.push_back(CMenuItem("Shutdown", NULL));
	menuItems.push_back(CMenuItem("Cancel", NULL));
	currentPosition = 2;
}

bool CRestartDlg::OnMenuItemSelected(int index)
{
	long vol = 0;
	switch (index)
	{
	case 0: // Reboot
		lcd.Clear();
		lcd.GotoXY(4, 1);
		lcd.PutS("Rebooting...");
		system("reboot");
		return true;
	case 1: // Shutdown
		lcd.Clear();
		lcd.GotoXY(2, 1);
		lcd.PutS("Shutting down...");
		system("shutdown 0");
		return true;
	}
	return false;
}

void CRestartDlg::Paint()
{
	CMenuBase::Paint();
	lcd.GotoXY(0, 0);
	lcd.PutS("Select action:");
}
