#include "ClientStream.h"
#include "PacketHeader.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SERVER_DATA
#include "pwd.h"

#define TEMP_HUM_SENSOR_IDX 25

int ClientStream::image_num = 0;
CDomoticzInterface ClientStream::iface("127.0.0.1", 8080);

void ClientStream::OnConnected()
{
}

bool ClientStream::OnDataReceived(unsigned char* buf, int len)
{
	packet.reserve(packet.size() + len);
	packet.insert(packet.end(), buf, buf + len);

	if (packet.size() > MAX_PACKET_SIZE)
	{
		printf("Too large packet:%d\n", packet.size());
		return false;
	}

	if (packet.size() >= sizeof(PacketHeader))
	{
		const PacketHeader* header = (const PacketHeader*)packet.data();
		if (header->Preamble != 0xCA3217AD || strncmp(header->Pwd, HOST_PWD, sizeof(header->Pwd)-1))
		{
			printf("Wrong preamble/password. Closing.\n");
			return false;
		}
		else if (packet.size() - sizeof(PacketHeader) >= header->PayloadLength )
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
				
				FILE* fp = fopen(fileName, "wb");
				if (fp)
				{
					printf("Saving image, len: %d\n", header->PayloadLength);
					fwrite(&packet[sizeof(PacketHeader)], header->PayloadLength, 1, fp);
					fclose(fp);
					return false;
				}
				else
				{
					printf("Error opening file %s", fileName);
					return false;
				}
			}
		}
	}
	return true;
}
