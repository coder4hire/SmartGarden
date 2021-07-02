#pragma once

#include <vector>
#include "DomoticzInterface.h"

#define MAX_PACKET_SIZE (5*1024*1024)
#define MAX_IMAGES_NUM 10
#define CONN_TIMEOUT_MS 180000ul

class ClientStream
{
public:
	void OnConnected();
	bool IsTimedOut();
	bool OnDataReceived(unsigned char* buf, int len);
	static int image_num;
	time_t openTime=0;

protected:
	std::vector<unsigned char> packet;
	static CDomoticzInterface iface;
};