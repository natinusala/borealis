#include <switch.h>
#include <unistd.h>

static int nxlink_sock = -1;
static void *haddr;
extern char *fake_heap_end;
extern u32 __nx_applet_type;

void userAppInit()
{   
    if (__nx_applet_type != AppletType_Application && __nx_applet_type != AppletType_SystemApplication) {
        // Setup Heap for swkbd on applets
        svcSetHeapSize(&haddr, 0x10000000);
        fake_heap_end = (char*) haddr + 0x10000000;
    }

    // romfsInit();
    socketInitializeDefault();
    nxlink_sock = nxlinkStdio();
    plInitialize();
    setsysInitialize();
}

void userAppExit()
{   
    if (__nx_applet_type != AppletType_Application && __nx_applet_type != AppletType_SystemApplication)
        svcSetHeapSize(&haddr, ((u8*) envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*) haddr); // clean up the heap

    if (nxlink_sock != -1)
        close(nxlink_sock);
    socketExit();
    // romfsExit();
    plExit();
    setsysExit();
}
