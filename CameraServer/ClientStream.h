#pragma once

#include <vector>

#define MAX_PACKET_SIZE (5*1024*1024)
#define MAX_IMAGES_NUM 5
class ClientStream
{
public:

	~ClientStream();
	void OnConnected();
	bool OnDataReceived(unsigned char* buf, int len);

protected:
	std::vector<unsigned char> packet;
	static int image_num;
};