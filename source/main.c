#include "Input.h"
#include "Video.h"
#include "WiiLibs.h"
#include <ogc/machine/processor.h>

#define VER "1.1"

// Header and Profiles :

#define PROFSIZE 0x91C
#define HEADERSIZE 0x8

// IP and DNS :

#define MANUALIPOFFSET 0x4
#define MANUALDNSOFFSET 0x10

// Wireless :

#define SSIDOFFSET 0x7C4
#define PASSKEYOFFSET 0x7F0
#define ENCRYPTIONTYPEOFFSET 0x7E9

// Proxy :

#define PROXYFLAGSOFFSET 0x24
#define PROXYSERVERNAMEOFFSET 0x28
#define PROXYPORTOFFSET 0x128
#define PROXYUSRNAMEOFFSET 0x12A
#define PROXYPASSWORDOFFSET 0x14B


#define aligned __attribute__((aligned(32)))

static fstats filest aligned;

void Value2Binary(u8 byte, bool *buffer) {
    for (int i = 7; i >= 0; i--) {
        if (byte >> i & 1) {
            buffer[7 - i] = true;
        } else {
            buffer[7 - i] = false;
        }
    }
}

char *decodeencryption(char byte) { // (Probably) Encryption.  OPN: 0x00, WEP64: 0x01, WEP128: 0x02 WPA-PSK (TKIP): 0x04, WPA2-PSK (AES): 0x05, WPA-PSK (AES): 0x06
    switch (byte) {
        case 0x00: 

            return "OPEN";
            
        break;

        case 0x01: 

            return "WEP64";
            
        break;

        case 0x02: 

            return "WEP128";
            
        break;

        case 0x04: 

            return "WPA-PSK (TKIP)";
            
        break;

        case 0x05: 

            return "WPA2-PSK (AES)";
            
        break;

        case 0x06: 

            return "WPA-PSK (AES)";
            
        break;
    
        default:
            return NULL;
        break;
    }
}

void printprofiledetails(int PROFNumber, const u8 *buff) {
    if (PROFNumber > 0 && PROFNumber < 4) {
        bool binary[8] = {false};

        Value2Binary(buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE)], binary);

        printf("<Connection %d>\n", PROFNumber);
        printf("Connection Selected? %s\n", binary[0] ? "Yes" : "No");
        printf("Use PMTU Recovery? %s\n", binary[1] ? "Yes" : "No");
        printf("Internet test passed? %s\n", binary[2] ? "Yes" : "No");
        printf("Use Proxy? %s\n", binary[3] ? "Yes" : "No");
        if (binary[3]) {
            printf("  + Proxy Server : ");
            for (size_t i = (HEADERSIZE + PROXYSERVERNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                putchar(buff[i]);
            }
            printf("\n  + Port : %d", (buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET] * (0x10) * (0x10)) + buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET + 1]);
            if (buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1]) {
                printf("\n  + Username : ");
                for (size_t i = (HEADERSIZE + PROXYUSRNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                    putchar(buff[i]);
                }
                printf("\n  + Password : ");
                for (size_t i = (HEADERSIZE + PROXYPASSWORDOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                    putchar(buff[i]);
                }
                putchar('\n');                
            }
        }
        printf("PPPoE? %s\n", binary[4] ? "Yes" : "No");
        printf("DNS Source? %s\n", binary[5] ? "Auto" : "Manual");
        if (!binary[5]) {
            printf("  + Primary DNS : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE)], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 2], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 3]);
            printf("  + Secondary DNS : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 4], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 5], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 6], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 7]);
        }
        printf("IP source? %s\n", binary[6] ? "DHCP" : "Manual");
        if (!binary[6]) {
            printf("  + IP Address : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE)], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 2], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 3]);
            printf("  + Subnet Mask : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 4], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 5], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 6], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 7]);
            printf("  + Router IP : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 8], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 9], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 10], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 11]);

        }
        printf("Interface? %s\n", binary[7] ? "Wired" : "Wireless");

        if (!binary[7]) {

            printf("\nSSID : ");

            for (size_t i = 0; buff[HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE) + i] != '\0'; i++)
            {
                putchar(buff[HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE) + i]);
            }
            

            printf("\nPASSKEY : ");

            for (size_t i = 0; buff[HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE) + i] != '\0'; i++)
            {
                putchar(buff[HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE) + i]);
            }

            printf("\nENCRYPTION : %s", decodeencryption(buff[HEADERSIZE + ENCRYPTIONTYPEOFFSET + (PROFNumber - 1) * PROFSIZE]));

        }
    }
}

u16 get_tmd_version(u64 title) {
    STACK_ALIGN(u8, tmdbuf, 1024, 32);
    u32 tmd_view_size = 0;
    s32 res;

    res = ES_GetTMDViewSize(title, &tmd_view_size);

    if (res < 0) return 0;

    if (tmd_view_size > 1024) return 0;

    ES_GetTMDView(title, (tmd_view*)tmdbuf, tmd_view_size);

    if (res < 0) return 0;

    return (tmdbuf[88] << 8) | tmdbuf[89];
}

int main() {
    
    int PROFNumber = 1;

    VideoInit();
    WPAD_Init();

    printf("Wii Connection Profiles Viewer %s\n By Abdelali221\n    This software is provided AS IT IS, so use it AT YOUR OWN RISK!\n Press anything but A if you want to exit...", VER);

    while (1) {
        int Input = CheckWPAD(0);
        if (Input == b_A) {
            break;
        } else if (Input != 0) {
            exit(0);
        }
    }

    ISFS_Initialize();

    ClearScreen();

    char cfgpath[64] = {0};

    u16 SMVER = get_tmd_version(0x0000000100000002);

    if (SMVER == 34 || SMVER == 33 || SMVER == 64) {
        sprintf(cfgpath, "/shared2/sys/net/config.dat");
    } else {
        sprintf(cfgpath, "/shared2/sys/net/02/config.dat");
    }
    

    s32 fcfg = ISFS_Open(cfgpath, ISFS_OPEN_READ);

    int stat = ISFS_GetFileStats(fcfg, &filest);
    if (fcfg >= 0 && stat == ISFS_OK) {
        printf("%s size : %d\n", cfgpath, filest.file_length);
    } else {
        printf("%d", stat);
    }

    usleep(2000000);

    ClearScreen();

    u8 aligned buff[filest.file_length + 1];

    for (size_t i = 0; i < sizeof(buff); i++)
    {
        buff[i] = 0;
    }
    

    ISFS_Read(fcfg, &buff, filest.file_length);

    printprofiledetails(PROFNumber, buff);

    ISFS_Close(fcfg);

    ISFS_Deinitialize();

    bool inloop = true;

    while(inloop) {
        int Input = CheckWPAD(0);

        if (Input == HOME) {
            inloop = false;
        }
            
        if (Input == LEFT) {
            if (PROFNumber > 1) {
                PROFNumber--;
                ClearScreen();
                printprofiledetails(PROFNumber, buff);
            }
        }

        if (Input == RIGHT) {
            if (PROFNumber < 3) {
                PROFNumber++;
                ClearScreen();
                printprofiledetails(PROFNumber, buff);
            }
        }
        VIDEO_WaitVSync();
    }
                
    ClearScreen();
    printf("Exiting...");
    return 0;
}