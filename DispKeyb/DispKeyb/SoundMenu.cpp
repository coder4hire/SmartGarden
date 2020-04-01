
#include "SoundMenu.h"
#include "HWInterface.h"
#include "stdio.h"

CSoundMenu::CSoundMenu()
{
	menuItems.push_back(CMenuItem("Volume Up", NULL));
	menuItems.push_back(CMenuItem("Volume Down", NULL));
	//menuItems.push_back(CMenuItem("Play", NULL));
	//menuItems.push_back(CMenuItem("Stop", NULL));

}

CSoundMenu::~CSoundMenu()
{
}

void CSoundMenu::Paint()
{
	CMenuBase::Paint();

	long vol = 0;
    CHWInterface::Inst.ALSAVolume(vol, true);
	lcd.GotoXY(17,0);
	lcd.Printf("%3d",vol);
}

void CSoundMenu::OnLeave()
{
	CHWInterface::Inst.SaveVolume();
}

bool CSoundMenu::OnMenuItemSelected(int index)
{
    long vol = 0;
	switch (index)
	{
	case 0: // Volume up
		CHWInterface::Inst.ALSAVolume(vol, true);
        vol = vol >= 90 ? 100 : vol + 10;
		CHWInterface::Inst.ALSAVolume(vol, false);
        break;
    case 1: // Volume down
		CHWInterface::Inst.ALSAVolume(vol, true);
        vol = vol <=10 ? 0 : vol - 10;
		CHWInterface::Inst.ALSAVolume(vol, false);
        break;
	}
	return true;
}
