#include <fat.h>
#include <network.h>
#include <ogc/machine/processor.h>
#include <sys/errno.h>

#include "cfgfile.h"
#include "Input.h"
#include "ncd.h"
#include "Video.h"
#include "WiiLibs.h"

#define VER "1.6"

int main() { 
    int PROFNumber = 1;

    VideoInit();
    InputInit();
    printf("Wii Network Profiles Viewer %s\n Created By Abdelali221", VER);
    POSCursor(21, 9);
    printf("This software makes changes to your");
    POSCursor(28, 10);
    printf("%sSystem configuration!%s", RED_BG_WHITE_FG, DEFAULT_BG_FG);
    POSCursor(26, 12);
    printf("Use it %sAT YOUR OWN RISK!%s", RED_BG_WHITE_FG, DEFAULT_BG_FG);
    POSCursor(16, 20);
    printf(" Press anything but + (L) if you want to exit...");
    POSCursor(14, 24);
    printf("Please check our guide before using this software :");
    POSCursor(16, 26);
    printf("https://abdelali221.github.io/guides/WNPV.html");

    int pressed;
    while (1)
    {
        pressed = CheckInput(0);
        if(pressed) break;
    }
    if(pressed != PLUS) {
        exit(0);
    }

    ClearScreen();

    netconfig_t aligned buff;

    memset(&buff, 0, sizeof(netconfig_t));

    printf("Reading Network configuration...");

    int ret = NCD_ReadConfig(&buff);
    
    if (ret < 0) {
        printf("Failed! ret = %d", ret);
        exit(0);
    }
    printf("Done.");
    sleep(2);
    ClearScreen();

    printprofiledetails(PROFNumber, &buff.connection[PROFNumber - 1]);

    bool inloop = true;

    while(inloop) {
        int Input = CheckInput(0);

        switch (Input) {
            case TWO:
                editprofile(PROFNumber, &buff);
                ret = NCD_ReadConfig(&buff);
                if(ret < 0) {
                    printf("Failed reading the configuration! ret = %d", ret);
                }
                printprofiledetails(PROFNumber, &buff.connection[PROFNumber - 1]);
            break;

            case HOME:
                inloop = false;
            break;

            case ONE:
                dumpfile(&buff, sizeof(buff), "/config.dat");
                ClearScreen();
                printprofiledetails(PROFNumber, &buff.connection[PROFNumber - 1]);
            break;

            case PLUS:
                ClearScreen();
                WPAD_Shutdown();
                printf("Reloading IOS...\n\n");
                IOS_ReloadIOS(IOS_GetVersion());
                printf("Starting Network test :");
                printf("\n\n This test only checks the connectivity between your Wii and your\n");
                printf("router, it doesn't check whether or not it's connected to the Internet!\n\n");
                printf("Initializing Network...\n");
                NCD_SetIpConfig(&buff);
                ret = -EBUSY;
                u8 retries = 7;
                while (retries > 0) {
                    retries--;
                    net_init_async(NULL, NULL);
                    ret = net_get_status();
                    while (ret == -EBUSY) {
                        usleep(50 * 1000);
                        ret = net_get_status();
                    }
                    if (ret != 0) {
                        printf ("\nnet_init failed: %d, trying again...", ret);
                        continue;
                    } else {
                        break;
                    }            
                }

                if(!ret) {
                    char ip[4];
                    u32 _ip = net_gethostip();
                    memcpy(ip, &_ip, 4);
                    printf("\nSuccess! %d\n IP : %d.%d.%d.%d", ret, ip[0], ip[1], ip[2], ip[3]);
                } else {
                    printf("Failed! Error %d", ret);
                }
                net_deinit();
                InputInit();
                sleep(3);
                ClearScreen();
                printprofiledetails(PROFNumber, &buff.connection[PROFNumber - 1]);
            break;

            case LEFT:
                if (PROFNumber > 1) {
                    PROFNumber--;
                    ClearScreen();
                    printprofiledetails(PROFNumber, &buff.connection[PROFNumber - 1]);
                }
            break;

            case RIGHT:
                if (PROFNumber < 3) {
                    PROFNumber++;
                    ClearScreen();
                    printprofiledetails(PROFNumber, &buff.connection[PROFNumber - 1]);
                }
            break;

            default:
            break;
        }            
        VIDEO_WaitVSync();
    }
                
    ClearScreen();
    printf("Exiting...");
    return 0;
}