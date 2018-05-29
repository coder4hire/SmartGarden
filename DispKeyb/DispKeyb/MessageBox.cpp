#include "MessageBox.h"
#include "string.h"

CMessageBox CMessageBox::Inst;

CMessageBox::CMessageBox()
{
}


CMessageBox::~CMessageBox()
{
	isYes = false;
}

bool CMessageBox::Show(const char* msg, CScreenBase* parent)
{
	isYes = false;
	message = msg;
	Run();
	if (parent)
	{
		parent->Paint();
	}
	return isYes;
}

bool CMessageBox::OnKeyPress(int key)
{
	switch(key)
	{ 
	case KEY_UP: 
		isYes = true;
		Paint();
		break;
	case KEY_DOWN:
		isYes = false;
		Paint();
		break;
	case KEY_OK:
		return false;
	case KEY_CANCEL:
		isYes = false;
		return false;
	}
	return key != KEY_CANCEL;
}

void CMessageBox::Paint()
{
	int len = strlen(message);
	lcd.GotoXY(len < 20 ? (20 - len) / 2 : 0, 0);
	lcd.PutS(message);

	if (isYes)
	{
		lcd.GotoXY(7, 2);
		lcd.PutS("> YES <");
		lcd.GotoXY(7, 3);
		lcd.PutS("   NO  ");
	}
	else
	{
		lcd.GotoXY(7, 2);
		lcd.PutS("  YES  ");
		lcd.GotoXY(7, 3);
		lcd.PutS(">  NO <");
	}
}