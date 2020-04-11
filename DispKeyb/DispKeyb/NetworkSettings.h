#pragma once
#include "MenuBase.h"

class CNetworkSettings :
	public CMenuBase
{
public:
	CNetworkSettings();
	~CNetworkSettings();

	bool OnMenuItemSelected(int index);

	virtual void Paint();
};

