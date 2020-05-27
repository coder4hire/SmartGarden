#pragma once
#include <sys/socket.h>
#include <sys/poll.h>
#include <vector>
#include <thread>
#include <atomic>
#include <map>
#include "ClientStream.h"

#define MAX_SOCKETS_NUM 10

class Server
{
public:
	Server();
	~Server();

	bool Listen();
	void Close();

protected:
	bool AcceptConnection(int socket);

	pollfd fds[MAX_SOCKETS_NUM + 1];
	unsigned int fds_len;
	std::map<int, ClientStream> clientStreams;
	std::atomic_bool shouldStop;
	int listener;
};

