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