#pragma once
class CHWInterface
{
public:
	static CHWInterface Inst;
	~CHWInterface();
	bool SetOutValue(unsigned int port, bool enable);

	bool GetOutStatus(unsigned int port);

protected:
	CHWInterface();
	static int OutNum2Pin[];
};

