#ifndef _CFGFILE_H_
#define _CFGFILE_H_

#include "WiiLibs.h"

// From Wiibrew.org With a few modifications

typedef struct _proxy // 327 bytes
{
    u8 use_proxy;               // 0x00 -> no proxy;  0x01 -> proxy
    u8 use_proxy_userandpass;    // 0x00 -> don't use username and password;  0x01 -> use username and password
    u8 padding_1[2];             // 0x00
    u8 proxy_name[255];
    u8 padding_2;               // 0x00
    u16 proxy_port;             // 0-34463 range
    u8 proxy_username[32];
    u8 padding_3;               // 0x00
    u8 proxy_password[32];
} proxy_t;

typedef struct _connection // 2332 bytes
{
    u8 flags;           // Defined below.
    u8 padding_1[3];

    u8 ip[4];           // Wii IP Address
    u8 netmask[4];
    u8 gateway[4];
    u8 dns1[4];
    u8 dns2[4];
    u8 padding_2[2];

    u16 mtu;            //valid values are 0 and 576-1500 range
    // 0x00 padding?

    proxy_t proxy_settings;
    u8 padding_4;       //0x00

    proxy_t proxy_settings_copy;    // Seems to be a duplicate of proxy_settings
    u8 padding_5[1302];             //0x00

    /*
     *  Wireless specific settings
     */
    u8 ssid[32];        // Access Point name.

    u8 padding_6;       // 0x00
    u8 ssid_length;     // length of ssid[] (AP name) in bytes.
    u8 padding_7[2];    // 0x00

    u8 padding_8;       // 0x00
    u8 encryption;      // (Probably) Encryption.  OPN: 0x00, WEP64: 0x01, WEP128: 0x02 WPA-PSK (TKIP): 0x04, WPA2-PSK (AES): 0x05, WPA-PSK (AES): 0x06
    u8 padding_9[2];    // 0x00

    u8 padding_10;      // 0x00
    u8 key_length;      // length of key[] (encryption key) in bytes.  0x00 for WEP64 and WEP128.
    u8 unknown;         // 0x00 or 0x01 toogled with a WPA-PSK (TKIP) and with a WEP entered with hex instead of ascii.
    u8 padding_11;      // 0x00

    u8 key[64];         // Encryption key.  For WEP, key is stored 4 times (20 bytes for WEP64 and 52 bytes for WEP128) then padded with 0x00.

    u8 padding_12[235]; // 0x00
} connection_t;

typedef struct _netconfig
{
    u32 version;       // 0x00
    u8 header4;        // 0x01  When there's at least one valid connection to the Internet.
    u8 header5;        // 0x00
    u8 linkTimeoutSec; // 0x07  Link timeout in seconds, usually 7.
    u8 padding;        // 0x00

    connection_t connection[3];
} netconfig_t;


// Flags :

#define INTERFACE 1
#define DNS 2
#define DHCP 4
#define PPPOE 8
#define USE_PROXY 16
#define TEST_PASSED 32
#define USE_PMTU 64
#define CONNECTION_SELECTED 128

// Security :

#define OPEN 0x00
#define WEP64 0x01
#define WEP128 0x02
#define WPA_PSK_TKIP 0x04
#define WPA2_PSK_AES 0x05
#define WPA_PSK_AES 0x06

#define SM1PATH "/shared2/sys/net/config.dat"

#define POSTSM1PATH "/shared2/sys/net/02/config.dat"

#define aligned __attribute__((aligned(32)))

void dumpfile(netconfig_t *buff, size_t length, const char *path);
void printprofiledetails(int PROFNumber, connection_t *profile);
void editprofile(int PROFNumber, netconfig_t buff, const char *cfgpath);

#endif