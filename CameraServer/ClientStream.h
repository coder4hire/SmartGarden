#pragma once
class ClientStream
{
public:

	~ClientStream();
	void OnConnected();
	bool OnDataReceived(int len);

	unsigned char Buffer[2048];
};

