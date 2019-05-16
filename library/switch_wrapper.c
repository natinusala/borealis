#include <switch.h>
#include <unistd.h>

static int nxlink_sock = -1;

void userAppInit()
{
    romfsInit();
    socketInitializeDefault();
    nxlink_sock = nxlinkStdio();
}

void userAppExit()
{
    if (nxlink_sock != -1)
        close(nxlink_sock);
    socketExit();
    romfsExit();
}
