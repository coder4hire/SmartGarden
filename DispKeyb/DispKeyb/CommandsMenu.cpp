#include "CommandsMenu.h"
#include "MessageBox.h"
#include "HWInterface.h"

CCommandsMenu::CCommandsMenu()
{
	menuItems.push_back(CMenuItem("Turn ON pump",NULL));
	menuItems.push_back(CMenuItem("Turn OFF pump",NULL));
	timerCount = 10;
}


CCommandsMenu::~CCommandsMenu()
{
}

void CCommandsMenu::Paint()
{
	CMenuBase::Paint();
	PaintStatus();
}

void CCommandsMenu::OnNonPreciseTimer()
{
	PaintStatus();
}

void CCommandsMenu::PaintStatus()
{
	lcd.GotoXY(0, 3);
	lcd.PutS(CHWInterface::Inst.GetOutStatus(1) ? "Pump ON " : "Pump OFF");
}

bool CCommandsMenu::OnMenuItemSelected(int index)
{
	switch(index)
	{
		case 0: // Turn Pump ON
			if (CMessageBox::Inst.Show("Turn pump ON ?"))
			{
				CHWInterface::Inst.SetOutValue(1, true);
			}
			break;
		case 1: // Turn Pump OFF
			if (CMessageBox::Inst.Show("Turn pump OFF ?"))
			{
				CHWInterface::Inst.SetOutValue(1, false);
			}
			break;
	}
	return true;
}
