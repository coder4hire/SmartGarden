#pragma once
class CHWInterface
{
public:
	static CHWInterface Inst;
	~CHWInterface();
	bool SetOutValue(unsigned int port, bool enable);

	bool GetOutStatus(unsigned int port);

	int ALSAVolume(long& vol, bool isReading);
	void SaveVolume();
	void RestoreVolume();

protected:
	CHWInterface();
	static int OutNum2Pin[];
};

