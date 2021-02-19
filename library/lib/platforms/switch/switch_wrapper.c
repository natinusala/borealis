/*
    Copyright 2019 natinusala
    Copyright 2019 WerWolv
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <switch.h>
#include <unistd.h>

static int nxlink_sock = -1;

void userAppInit()
{
    romfsInit();
    socketInitializeDefault();
    plInitialize(PlServiceType_User);
    setsysInitialize();
    setInitialize();

    nxlink_sock = nxlinkStdio();
}

void userAppExit()
{
    if (nxlink_sock != -1)
        close(nxlink_sock);

    socketExit();
    romfsExit();
    plExit();
    setsysExit();
    setExit();
}
