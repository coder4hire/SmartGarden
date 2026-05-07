#include "ClientStream.h"
#include "PacketHeader.h"
#include "DbgOut.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define SERVER_DATA
#include "pwd.h"

#define TEMP_HUM_SENSOR_IDX 25

int ClientStream::image_num = 0;
CDomoticzInterface ClientStream::iface("127.0.0.1", 8080);

void ClientStream::OnConnected()
{
	time(&openTime);
}

bool ClientStream::IsTimedOut()
{
	time_t now;
	time(&now);
	return !openTime || now - openTime > CONN_TIMEOUT_S;
}

bool ClientStream::OnDataReceived(unsigned char* buf, int len)
{
	packet.reserve(packet.size() + len);
	packet.insert(packet.end(), buf, buf + len);

	if (packet.size() > MAX_PACKET_SIZE)
	{
		dbgPrintf("Too large packet:%d\n", packet.size());
		return false;
	}

	if (packet.size() >= sizeof(PacketHeader))
	{
		const PacketHeader* header = (const PacketHeader*)packet.data();
		dbgPrintf("Header.Payload: %d PacketPayload: %d.\n",header->PayloadLength, packet.size() - sizeof(PacketHeader));
		if (header->Preamble != 0xCA3217AD || strncmp(header->Pwd, HOST_PWD, sizeof(header->Pwd)-1))
		{
			errPrintf("Wrong preamble/password. Closing.\n");
			return false;
		}
		else if (packet.size() >= header->PayloadLength + sizeof(PacketHeader))
		{
			//---  Sending Temperature/Humidity
			if (!isnan(header->Temperature) && !isnan(header->Humidity))
			{
				iface.SendTempHumidity(TEMP_HUM_SENSOR_IDX, header->Temperature, header->Humidity);
			}

			if (header->PayloadLength > 0)
			{
				//--- Saving image to file
				char fileName[128];
				snprintf(fileName, 127, "/tmp/cam0/%02d.jpg", image_num);
				image_num= (image_num+1) % MAX_IMAGES_NUM;
				
				FILE* fp;
				for (int retr = 0; !(fp = fopen(fileName, "wb")) && retr < 10; retr++)
				{
					sleep(1);
				}
				if (fp)
				{
					dbgPrintf("Saving image, len: %d\n", header->PayloadLength);
					fwrite(&packet[sizeof(PacketHeader)], header->PayloadLength, 1, fp);
					fclose(fp);
					openTime = 0;
					return false;
				}
				else
				{
					errPrintf("Error opening file %s\n", fileName);
					openTime = 0;
					return false;
				}
			}
		}
	}
	return true;
}

std::string ClientStream::GetRcvdInfo()
{
	return "Rcvd:" + std::to_string(packet.size()) +
		(packet.size() >= sizeof(PacketHeader) ?
		std::string(" / Hdr payload:") + std::to_string(((const PacketHeader*)packet.data())->PayloadLength) :
		std::string(" / No header received yet"));
}
