#pragma once
#include "ScreenBase.h"

class CNetworkSettings :
	public CScreenBase
{
public:
	CNetworkSettings();
	~CNetworkSettings();

	virtual bool OnKeyPress(int key);

	virtual void Paint();
};

