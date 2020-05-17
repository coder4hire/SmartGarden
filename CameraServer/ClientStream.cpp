
#include "ClientStream.h"

ClientStream::~ClientStream()
{
}

void ClientStream::OnConnected()
{
}

bool ClientStream::OnDataReceived(int len)
{
	return true;
}
