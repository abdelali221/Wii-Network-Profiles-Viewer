#ifndef _CFGFILE_H_
#define _CFGFILE_H_

#include <stdint.h>
#include <stddef.h>


// Header and Profiles :

#define PROFSIZE 0x91C
#define HEADERSIZE 0x8

// IP and DNS :

#define MANUALIPOFFSET 0x4
#define MANUALDNSOFFSET 0x10

// Wireless :

#define SSIDOFFSET 0x7C4
#define SSIDSIZEOFFSET 0x7E5
#define PASSKEYOFFSET 0x7F0
#define PASSKEYSIZEOFFSET 0x7ED
#define ENCRYPTIONTYPEOFFSET 0x7E9

// Proxy :

#define ALTPROXYOFFSET 0x148
#define PROXYFLAGSOFFSET 0x24
#define PROXYSERVERNAMEOFFSET 0x28
#define PROXYPORTOFFSET 0x128
#define PROXYUSRNAMEOFFSET 0x12A
#define PROXYPASSWORDOFFSET 0x14B

#define SM1PATH "/shared2/sys/net/config.dat"

#define POSTSM1PATH "/shared2/sys/net/02/config.dat"

#define aligned __attribute__((aligned(32)))

void Value2Binary(uint8_t *byte, bool *buffer, bool mode);
void dumpfile(void *buff, size_t length, const char *path);
void printprofiledetails(int PROFNumber, uint8_t *buff);
void editprofile(int PROFNumber, uint8_t *buff, const char *cfgpath);

#endif