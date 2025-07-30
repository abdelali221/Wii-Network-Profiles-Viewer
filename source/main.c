#include "Input.h"
#include "Video.h"
#include "WiiLibs.h"

#define VER "1.1"

#define PROFSIZE 0x91C
#define HEADERSIZE 0x8
#define SSIDOFFSET 0x7C4
#define PASSKEYOFFSET 0x7F0
#define ENCRYPTIONTYPEOFFSET 0x7E9
#define MANUALIPOFFSET 0x4
#define MANUALDNSOFFSET 0x10

#define aligned __attribute__((aligned(32)))

static fstats filest aligned; 

int PROFNumber = 1;

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

int main() {
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

    s32 fcfg = ISFS_Open("/shared2/sys/net/02/config.dat", ISFS_OPEN_READ);

    int stat = ISFS_GetFileStats(fcfg, &filest);
    if (fcfg >= 0 && stat == ISFS_OK) {
        printf("/shared2/sys/net/02/config.dat size : %d\n", filest.file_length);
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