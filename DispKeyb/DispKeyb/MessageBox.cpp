#include "MessageBox.h"
#include "string.h"

CMessageBox CMessageBox::Inst;

CMessageBox::CMessageBox()
{
	answer = OK;
}


CMessageBox::~CMessageBox()
{
}

bool CMessageBox::Show(const char* msg, CScreenBase* parent, bool askYesNo)
{
	answer = askYesNo ? NO : OK;
	message = msg;
	Run();
	if (parent)
	{
		parent->Paint();
	}
	return answer == YES;
}

bool CMessageBox::OnKeyPress(int key)
{
	switch(key)
	{ 
	case KEY_UP: 
		if (answer != OK)
		{
			answer = YES;
			Paint();
		}
		break;
	case KEY_DOWN:
		if (answer != OK)
		{
			answer = NO;
			Paint();
		}
		break;
	case KEY_OK:
		return false;
	case KEY_CANCEL:
		if (answer != OK)
		{
			answer = NO;
		}
		return false;
	}
	return key != KEY_CANCEL;
}

void CMessageBox::Paint()
{
	int len = strlen(message);
	lcd.GotoXY(len < 20 ? (20 - len) / 2 : 0, 0);
	lcd.PutS(message);

	switch(answer)
	{
	case YES:
		lcd.GotoXY(7, 2);
		lcd.PutS("> YES <");
		lcd.GotoXY(7, 3);
		lcd.PutS("   NO  ");
		break;
	case NO:
		lcd.GotoXY(7, 2);
		lcd.PutS("  YES  ");
		lcd.GotoXY(7, 3);
		lcd.PutS(">  NO <");
		break;
	default: 
		lcd.GotoXY(7, 2);
		lcd.PutS("  OK   ");
	}
}