#include "Input.h"
#include "Video.h"
#include "WiiLibs.h"
#include <ogc/machine/processor.h>
#include "virtualkb.h"
#include "cfgfile.h"

#define VER "1.3"

static fstats filest aligned;

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
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);

    printf("Wii Network Profiles Viewer %s\n Written By Abdelali221", VER);
    POSCursor(20, 10);
    printf("This software writes to the %sNAND!%s", RED_BG_WHITE_FG, DEFAULT_BG_FG);
    POSCursor(23, 12);
    printf("Use it %sAT YOUR OWN RISK!%s", RED_BG_WHITE_FG, DEFAULT_BG_FG);
    POSCursor(16, 20);
    printf(" Press anything but + if you want to exit...");

    while (1) {
        int Input = CheckWPAD(0);
        if (Input == PLUS) {
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
        sprintf(cfgpath, SM1PATH);
    } else {
        sprintf(cfgpath, POSTSM1PATH);
    }

    printf("%s ", cfgpath);
    
    s32 fcfg = ISFS_Open(cfgpath, ISFS_OPEN_READ);

    int stat = ISFS_GetFileStats(fcfg, &filest);
    if (fcfg >= 0 && stat == ISFS_OK) {
        printf("size : %d\n", filest.file_length);
    } else {
        if (stat == -4) {
            printf("NOT FOUND!\n");
            if (!strcmp(cfgpath, SM1PATH)) {
                sprintf(cfgpath, POSTSM1PATH);
                fcfg = ISFS_Open(cfgpath, ISFS_OPEN_READ);
            }
            if (!strcmp(cfgpath, POSTSM1PATH) == 0) {
                sprintf(cfgpath, SM1PATH);
                fcfg = ISFS_Open(cfgpath, ISFS_OPEN_READ);
            }
            printf("%s ", cfgpath);
        }
        stat = ISFS_GetFileStats(fcfg, &filest);
        if (fcfg >= 0 && stat == ISFS_OK) {
            printf("size : %d\n", filest.file_length);
        } else {
            printf(" NOT FOUND!\nAborting...");
            exit(0);
        }
    }

    sleep(2);

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

        switch (Input) {
            case TWO:
                editprofile(PROFNumber, buff, cfgpath);
                ISFS_Initialize();
                s32 fcfg = ISFS_Open(cfgpath, ISFS_OPEN_READ);
                ISFS_Read(fcfg, &buff, filest.file_length);
                ISFS_Close(fcfg);
                ISFS_Deinitialize();
            break;

            case HOME:
                inloop = false;
            break;

            case ONE:
                dumpfile(buff, sizeof(buff), "sd:/config.dat");
                ClearScreen();
                printprofiledetails(PROFNumber, buff);
            break;

            case LEFT:
                if (PROFNumber > 1) {
                    PROFNumber--;
                    ClearScreen();
                    printprofiledetails(PROFNumber, buff);
                }
            break;

            case RIGHT:
                if (PROFNumber < 3) {
                    PROFNumber++;
                    ClearScreen();
                    printprofiledetails(PROFNumber, buff);
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