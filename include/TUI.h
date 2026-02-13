#ifndef _TUI_H_
#define _TUI_H_

#include "WiiLibs.h"

void printtabletop();
void printtablebottom();
void ReadString(u8* dest, u8* len, u8 maxlen);
u32 ReadNumString(u32 maxval);
void ReadDNS_IP(u8* dest);
#endif