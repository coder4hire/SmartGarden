#pragma once

#include <vector>
#include "DomoticzInterface.h"

#define MAX_PACKET_SIZE (5*1024*1024)
#define MAX_IMAGES_NUM 10
class ClientStream
{
public:
	void OnConnected();
	bool OnDataReceived(unsigned char* buf, int len);
	static int image_num;

protected:
	std::vector<unsigned char> packet;
	static CDomoticzInterface iface;
};