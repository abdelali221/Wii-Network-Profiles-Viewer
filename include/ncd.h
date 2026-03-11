#ifndef _NCD_H_
#define _NCD_H_

#include "WiiLibs.h"
#include "cfgfile.h"

typedef s32 NCD_Status;

NCD_Status NCD_GetStatus();
int NCD_SetIpConfig(netconfig_t* buff);
int NCD_WriteConfig(netconfig_t* buff);
int NCD_ReadConfig(netconfig_t* buff);

#endif