#include "MessageBox.h"
#include "string.h"

CMessageBox CMessageBox::Inst;

CMessageBox::CMessageBox()
{
}


CMessageBox::~CMessageBox()
{
}

bool CMessageBox::Show(const char* msg, CScreenBase* parent)
{
	message = msg;
	Run();
	if (parent)
	{
		parent->Paint();
	}
}

bool CMessageBox::OnKeyPress(int key)
{
	return key != KEY_CANCEL;
}

void CMessageBox::Paint()
{
	int len = strlen(message);
	lcd.GotoXY(len < 20 ? (20 - len) / 2 : 0, 1);
	lcd.PutS(message);
}