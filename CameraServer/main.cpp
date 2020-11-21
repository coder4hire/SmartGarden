#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <memory.h>
#include "Server.h"
#include "sys/stat.h"
#include "ClientStream.h"
#include <dirent.h>

Server server;

void sig_term_handler(int signum, siginfo_t* info, void* ptr)
{
    printf("Shutting down server...\n");
    fflush(stdout);
    server.Close();
}

int countFiles(const char* dirPath)
{
    DIR* dir;
    struct dirent* ent;
    int count = 0;
    if ((dir = opendir(dirPath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG)
            {
                count++;
            }
        }
        closedir(dir);
    }
    return count;
}

int main()
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_term_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &_sigact, NULL);

    mkdir("/tmp/cam0", 0777);

    ClientStream::image_num = countFiles("/tmp/cam0");

    server.Listen();
	return 0;
}