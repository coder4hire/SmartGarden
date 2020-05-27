#pragma once

#pragma pack(push, 1)
struct PacketHeader
{
	unsigned long Preamble;
	char Pwd[16];
	long Temperature;
	long Humidity;
	unsigned long PayloadLength;
};
#pragma pack(pop)
