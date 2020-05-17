#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <memory.h>
#include "Server.h"

Server server;

void sig_term_handler(int signum, siginfo_t* info, void* ptr)
{
    server.Close();
}

int main()
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_term_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &_sigact, NULL);
    
    server.Listen();
	return 0;
}