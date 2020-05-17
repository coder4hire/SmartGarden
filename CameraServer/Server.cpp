#include "Server.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>

Server::Server()
{
}

Server::~Server()
{
}

bool Server::Listen()
{
	shouldStop = false;
    unsigned long filter_ip = htonl(0xC0A80328);

    memset(fds, 0, sizeof(fds));

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return false;
    }

    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    //bind the socket to localhost port 8888
    if (bind(listener, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind failed");
        return false;
    }

    if (listen(listener, 3) < 0)
    {
        perror("listen error");
        return false;
    }

    //accept the incoming connection
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    fds_len = 1;

    while (!shouldStop)
    {
        if (poll(fds, fds_len, 100) > 0)
        {
            // Checking incoming connection
            if (fds[0].revents == POLLIN)
            {
                int client_socket;
                struct sockaddr_in peer_addr = {};
                socklen_t addrlen = sizeof(peer_addr);

                if ((client_socket = accept(listener, (struct sockaddr*) & peer_addr, (socklen_t*)&addrlen)) < 0)
                {
                    if (peer_addr.sin_addr.s_addr == filter_ip)
                    {
                        AcceptConnection(client_socket);
                    }
                    else
                    {
                        close(client_socket);
                    }
                }
            }

            // Checking incoming data
            for (int i = 1; i < fds_len; i++)
            {
                if (fds[i].revents == POLLIN)
                {
                    std::map<int, ClientStream>::iterator stream_it = clientStreams.find(fds[i].fd);
                    if (stream_it != clientStreams.end())
                    {
                        ClientStream& str = stream_it->second;
                        int rc = recv(fds[i].fd, str.Buffer, sizeof(str.Buffer), 0);
                        if (rc < 0)
                        {
                            // Close connection
                            clientStreams.erase(stream_it);
                            fds[i].fd = 0;
                        }
                        else
                        {
                            if (!str.OnDataReceived(rc))
                            {
                                // Close connection if Data processing is over
                                clientStreams.erase(stream_it);
                                fds[i].fd = 0;
                            }
                        }
                    }
                }
            }

            int shift = 0;
            for (int i = 1; i < fds_len; i++)
            {
                if (!fds[i].fd)
                {
                    shift++;
                }
                else if(shift)
                {
                    fds[i - shift] = fds[i];
                }
            }
            fds_len -= shift;
        }
    }

    return true;
}

void Server::Close()
{
    shouldStop = true;
}

bool Server::AcceptConnection(int socket)
{
    if (fds_len < MAX_SOCKETS_NUM + 1)
    {
        fds[fds_len].fd = socket;
        fds[fds_len].events = POLLIN;
        clientStreams[socket] = ClientStream();
        return true;
    }
    return false;
}
