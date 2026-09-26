/*-----------------------------------------------------------------------

    Wii-Network-Profiles-Viewer --
        A network profiles viewer/editor for the Nintendo Wii

    https://github.com/abdelali221/Wii-Network-Profiles-Viewer

    Copyright (C) 2025 - 2026 Abdelali221

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

-----------------------------------------------------------------------*/

#include "ncd.h"

NCD_Status ncdcommonstatus;

NCD_Status NCD_GetStatus() {
    return ncdcommonstatus;
}

int NCD_WriteConfig(netconfig_t* buff) {
    ioctlv vectors[2];

    vectors[0].data = buff;
    vectors[1].data = &ncdcommonstatus;

    vectors[0].len = 7004;
    vectors[1].len = 32;
    
    int fd = IOS_Open("/dev/net/ncd/manage", IPC_OPEN_NONE);

    if(!fd) return -1;

    int ret = IOS_Ioctlv(fd, 6, 1, 1, vectors);

    IOS_Close(fd);

    return ret;
}

int NCD_SetIpConfig(netconfig_t* buff) {
    ioctlv vectors[2];

    vectors[0].data = buff;
    vectors[1].data = &ncdcommonstatus;

    vectors[0].len = 7004;
    vectors[1].len = 32;
    
    int fd = IOS_Open("/dev/net/ncd/manage", IPC_OPEN_NONE);

    if(!fd) return -1;

    int ret = IOS_Ioctlv(fd, 3, 1, 1, vectors);

    IOS_Close(fd);

    return ret;
}

int NCD_ReadConfig(netconfig_t* buff) {
    ioctlv vectors[2];
    vectors[0].data = buff;
    vectors[1].data = &ncdcommonstatus;

    vectors[0].len = 7004;
    vectors[1].len = 32;
    
    int fd = IOS_Open("/dev/net/ncd/manage", IPC_OPEN_NONE);

    if(!fd) return -1;
    int ret = IOS_Ioctlv(fd, 5, 0, 2, vectors);

    IOS_Close(fd);
    return ret;
}