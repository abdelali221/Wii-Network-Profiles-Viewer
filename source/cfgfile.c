#include "cfgfile.h"
#include "WiiLibs.h"
#include "Video.h"
#include <fat.h>
#include <math.h>
#include "Input.h"
#include "virtualkb.h"

void Value2Binary(uint8_t *byte, bool *buffer, bool mode) {
    if(mode) {
        for (int i = 7; i >= 0; i--) {
            if (*byte >> i & 1) {
                buffer[7 - i] = true;
            } else {
                buffer[7 - i] = false;
            }
        }
    } else {
        *byte = 0;
        for (size_t i = 0; i < 8; i++) {   
            if(buffer[7 - i]) *byte += (int)pow(2, i);
        }        
    }
}

char *decodeencryption(char byte) {
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

void dumpfile(void *buff, size_t length, const char *path) {
    fatInitDefault();
    FILE* dmpf = fopen(path, "w+");
    if(!dmpf) return;

    fwrite(buff, length, sizeof(char), dmpf);

    fclose(dmpf);

    ClearScreen();

    printf("Successfully dumped!");
    sleep(2);
}

void printprofiledetails(int PROFNumber, uint8_t *buff) {
    if (PROFNumber > 0 && PROFNumber < 4) {
        bool binary[8] = {false};

        Value2Binary(&buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE)], binary, true);

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
            }
            putchar('\n'); 
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

            for (size_t i = (HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                putchar(buff[i]);
            }

            printf("\nPASSKEY : ");

            for (size_t i = 0; i < buff[HEADERSIZE + PASSKEYSIZEOFFSET + ((PROFNumber - 1) * PROFSIZE)]; i++) {
                putchar(buff[HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE) + i]);
            }

            printf("\nENCRYPTION : %s", decodeencryption(buff[HEADERSIZE + ENCRYPTIONTYPEOFFSET + (PROFNumber - 1) * PROFSIZE]));
        }
    }

    POSCursor(0, 26);
    printf(" 1 : Dump config.dat\n");
    printf(" 2 : Edit current profile");
}

void togglebit(bool *buffer, uint8_t bit) {
    if(buffer[bit]) buffer[bit] = false;
    else buffer[bit] = true;
}

void editproxy(int PROFNumber, uint8_t *buff) {
    int Selection = 0;
    while (1) {
        ClearScreen();

        POSCursor(30, 4);
        printf("Proxy Settings :");

        POSCursor(20, 6);
        printf("Proxy Server : ");
        for (size_t i = (HEADERSIZE + PROXYSERVERNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
            putchar(buff[i]);
        }

        POSCursor(20, 8);
        printf("Port : %d", (buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET] * (0x10) * (0x10)) + buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET + 1]);

        POSCursor(20, 10);
        printf("Needs Username/Password? %s", buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1] ? "Yes" : "No");

        POSCursor(20, 12);
        printf("Username : ");
        for (size_t i = (HEADERSIZE + PROXYUSRNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
            putchar(buff[i]);
        }

        POSCursor(20, 14);
        printf("Password : ");
        for (size_t i = (HEADERSIZE + PROXYPASSWORDOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
            putchar(buff[i]);
        }

        POSCursor(0, 26);
        printf("A : Edit\n");
        printf("HOME : Go back\n");
        
        POSCursor(18, 6 + (Selection * 2));
        printf("->");
        
        bool brk = true;

        while (brk) {
            int Input = CheckWPAD(0);

            switch (Input) {

                case HOME:
                    return;
                break;

                case DOWN:
                    if(Selection < 4) {
                        POSCursor(18, 6 + (Selection * 2));
                        printf("  ");
                        Selection++;
                        POSCursor(18, 6 + (Selection * 2));
                        printf("->");
                    }
                break;

                case UP:
                    if (Selection > 0) {
                        POSCursor(18, 6 + (Selection * 2));
                        printf("  ");
                        Selection--;
                        POSCursor(18, 6 + (Selection * 2));
                        printf("->");
                    }
                break;

                case b_A:
                    POSCursor(0, 26);
                    printf("\x1b[2K");
                    bool brk2 = true;
                    int idx = 0;
                    switch (Selection) {
                        case 0:
                            for (size_t i = 0; buff[(HEADERSIZE + PROXYSERVERNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)) + i] != '\0'; i++) {
                                idx++;
                            }
                            POSCursor(35 + idx, 6);
                            
                            while (brk2) {
                                static bool shift = false;
                                static int conX, conY;
                                int Input = CheckWPAD(0);
                                WPADData* data = WPAD_Data(0);
                                int irX = (((data->ir.x) * 76) / 560);
                                int irY = (((data->ir.y) * 28) / 420);
                                CON_GetPosition(&conX, &conY);
                                char chr = keyboard(shift, irX, irY);
                                POSCursor(conX, conY);
                                VIDEO_WaitVSync();

                                switch (Input) {
                                    case b_B:
                                        if(shift) shift = false;
                                        else shift = true;
                                    break;

                                    case HOME:
                                        ClearKeyboard();
                                        brk2 = false;
                                        brk = false;
                                    break;

                                    case b_A:
                                        if ((chr != '\0' && idx < 32) || chr == '\b') {
                                            switch (chr)
                                            {
                                                case '\b':
                                                    if (idx > 0) {
                                                        printf("\b \b");
                                                        idx--;
                                                        buff[(HEADERSIZE + PROXYSERVERNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                        buff[(HEADERSIZE + PROXYSERVERNAMEOFFSET + ALTPROXYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                    }
                                                case ' ':
                                                break;
                                            
                                                default:
                                                    putchar(chr);
                                                    buff[(HEADERSIZE + PROXYSERVERNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    buff[(HEADERSIZE + PROXYSERVERNAMEOFFSET + ALTPROXYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    idx++;                                                    
                                                break;
                                            }
                                        }
                                    break;
                                
                                    default:
                                    break;
                                }
                            }
                        break;

                        case 1:
                            POSCursor(27, 8);
                            printf("     \b\b\b\b\b");
                            u16 proxy = 0;                            
                            while (brk2) {                                
                                static int conX, conY;
                                int Input = CheckWPAD(0);
                                WPADData* data = WPAD_Data(0);
                                int irX = (((data->ir.x) * 76) / 560);
                                int irY = (((data->ir.y) * 28) / 420);
                                CON_GetPosition(&conX, &conY);
                                char chr = keyboard(true, irX, irY);
                                POSCursor(conX, conY);
                                VIDEO_WaitVSync();

                                switch (Input) {

                                    case ONE:
                                        buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET] = (uint8_t)(proxy >> 8);;
                                        buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET + 1] = (uint8_t)(proxy & 0xFF);
                                        ClearKeyboard();
                                        brk2 = false;
                                        brk = false;
                                    break;

                                    case b_A:
                                        if ((chr != '\0' && chr >= '0' && chr <= '9') || chr == '\b') {
                                            switch (chr) {
                                                case '\b':
                                                    if (idx > 0) {
                                                        printf("\b \b");
                                                        proxy /= 10;
                                                    }
                                                break;

                                                default:
                                                    if((proxy * 10) + chr - 48 <= 34463 && idx < 5){
                                                        putchar(chr);
                                                        proxy *= 10;
                                                        proxy += chr - 48;
                                                        idx++;
                                                    }                              
                                                break;
                                            }
                                        }
                                    break;
                                
                                    default:
                                    break;
                                }
                            }
                        break;

                        case 2:
                            if (buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1]) buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1] = 0;
                            else buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1] = 1;

                            brk2 = false;
                            brk = false;
                        break;

                        case 3:
                            for (size_t i = 0; buff[(HEADERSIZE + PROXYUSRNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)) + i] != '\0'; i++) {
                                idx++;
                            }
                            POSCursor(31 + idx, 12);
                            
                            while (brk2) {
                                static bool shift = false;
                                static int conX, conY;
                                int Input = CheckWPAD(0);
                                WPADData* data = WPAD_Data(0);
                                int irX = (((data->ir.x) * 76) / 560);
                                int irY = (((data->ir.y) * 28) / 420);
                                CON_GetPosition(&conX, &conY);
                                char chr = keyboard(shift, irX, irY);
                                POSCursor(conX, conY);
                                VIDEO_WaitVSync();

                                switch (Input) {
                                    case b_B:
                                        if(shift) shift = false;
                                        else shift = true;
                                    break;

                                    case HOME:
                                        ClearKeyboard();
                                        brk2 = false;
                                        brk = false;
                                    break;

                                    case b_A:
                                        if ((chr != '\0' && idx < 32) || chr == '\b') {
                                            switch (chr)
                                            {
                                                case '\b':
                                                    if (idx > 0) {
                                                        printf("\b \b");
                                                        idx--;
                                                        buff[(HEADERSIZE + PROXYUSRNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                        buff[(HEADERSIZE + PROXYUSRNAMEOFFSET + ALTPROXYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                    }
                                                case ' ':
                                                break;
                                            
                                                default:
                                                    putchar(chr);
                                                    buff[(HEADERSIZE + PROXYUSRNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    buff[(HEADERSIZE + PROXYUSRNAMEOFFSET + ALTPROXYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    idx++;                                                    
                                                break;
                                            }
                                        }
                                    break;
                                
                                    default:
                                    break;
                                }
                            }
                        break;

                        case 4:
                            for (size_t i = 0; buff[(HEADERSIZE + PROXYPASSWORDOFFSET + ((PROFNumber - 1) * PROFSIZE)) + i] != '\0'; i++) {
                                idx++;
                            }
                            POSCursor(31 + idx, 14);
                            
                            while (brk2) {
                                static bool shift = false;
                                static int conX, conY;
                                int Input = CheckWPAD(0);
                                WPADData* data = WPAD_Data(0);
                                int irX = (((data->ir.x) * 76) / 560);
                                int irY = (((data->ir.y) * 28) / 420);
                                CON_GetPosition(&conX, &conY);
                                char chr = keyboard(shift, irX, irY);
                                POSCursor(conX, conY);
                                VIDEO_WaitVSync();

                                switch (Input) {
                                    case b_B:
                                        if(shift) shift = false;
                                        else shift = true;
                                    break;

                                    case HOME:
                                        ClearKeyboard();
                                        brk2 = false;
                                        brk = false;
                                    break;

                                    case b_A:
                                        if ((chr != '\0' && idx < 32) || chr == '\b') {
                                            switch (chr)
                                            {
                                                case '\b':
                                                    if (idx > 0) {
                                                        printf("\b \b");
                                                        idx--;
                                                        buff[(HEADERSIZE + PROXYPASSWORDOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                        buff[(HEADERSIZE + PROXYPASSWORDOFFSET + ALTPROXYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                    }
                                                case ' ':
                                                break;
                                            
                                                default:
                                                    putchar(chr);
                                                    buff[(HEADERSIZE + PROXYPASSWORDOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    buff[(HEADERSIZE + PROXYPASSWORDOFFSET + ALTPROXYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    idx++;                                                    
                                                break;
                                            }
                                        }
                                    break;
                                
                                    default:
                                    break;
                                }
                            }
                        break;
                    
                        default:
                        break;
                    }
                break;
            }
        }
    }    
}

void editdns_ip(int PROFNumber, uint8_t *buff, bool dns_ip) {
    int Selection = 0;
    while (1) {
        ClearScreen();

        POSCursor(30, 5);
        if(dns_ip) {
            printf("IP Settings : ");
        } else {
            printf("DNS Settings : ");
        }

        POSCursor(20, 8);
        if(dns_ip) {
            printf("IP Address : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE)], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 2], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 3]);
        } else {
            printf("Primary DNS : ");
            for (size_t i = (HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE)); i < (HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE)) + 4; i++) {
                printf("%d.", buff[i]);
            }
            printf("\b \b");
        }
        
        POSCursor(20, 10);
        if(dns_ip) {
            printf("Subnet Mask : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 4], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 5], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 7], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 7]);
        } else {
            printf("Secondary DNS : ");
            for (size_t i = (HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE)) + 4; i < (HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE)) + 8; i++) {
                printf("%d.", buff[i]);
            }
            printf("\b \b");
        }

        if (dns_ip) {
            POSCursor(20, 12);
            printf("Router IP : %d.%d.%d.%d", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 8], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 9], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 10], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 11]);
        }

        POSCursor(0, 26);
        printf("A : Edit\n");
        printf("HOME : Go back\n");

        POSCursor(18, 8 + (Selection * 2));
        printf("->");
        u16 valbuff[4] = {0};
        
        bool brk = true;
        
        while (brk) {
            int Input = CheckWPAD(0);
            switch (Input) {

                case HOME:
                    return;
                break;

                case DOWN:
                    if((dns_ip && Selection < 2) || (!dns_ip && Selection < 1)) {
                        POSCursor(18, 8 + (Selection * 2));
                        printf("  ");
                        Selection++;
                        POSCursor(18, 8 + (Selection * 2));
                        printf("->");
                    }
                break;

                case UP:
                    if (Selection > 0) {
                        POSCursor(18, 8 + (Selection * 2));

                         printf("  ");
                        Selection--;
                        POSCursor(18, 8 + (Selection * 2));
                        printf("->");
                    }
                break;

                case b_A:
                    POSCursor(0, 26);
                    printf("\x1b[2K");
                    bool brk2 = true;
                    int idx = 0;
                    int i = 0; 
                    switch (Selection)
                    {
                        case 0:
                            if(dns_ip) POSCursor(33, 8);
                            else POSCursor(34, 8);
                        break;

                        case 1:
                            if(dns_ip) POSCursor(34, 10);
                            else POSCursor(36, 10);
                        break;

                        case 2:
                            POSCursor(32, 12);
                        break;
                    
                        default:
                        break;
                    }
                    
                    printf("%s%d%s.%d.%d.%d        ", WHITE_BG_BLACK_FG, valbuff[0], DEFAULT_BG_FG, valbuff[1], valbuff[2], valbuff[3]);
                    brk2 = true;                    
                    while (brk2) {                                
                        static int conX, conY;
                        int Input = CheckWPAD(0);
                        WPADData* data = WPAD_Data(0);
                        int irX = (((data->ir.x) * 76) / 560);
                        int irY = (((data->ir.y) * 28) / 420);
                        CON_GetPosition(&conX, &conY);
                        char chr = keyboard(true, irX, irY);
                        POSCursor(conX, conY);
                        VIDEO_WaitVSync();

                        switch (Input) {

                            case HOME:
                                for (size_t i = 0; i < 4; i++)
                                {
                                    if (dns_ip) {
                                        buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + i + (Selection * 4)] = valbuff[i];
                                    } else {
                                        buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + i + (Selection * 4)] = valbuff[i];
                                    }
                                }
                                ClearKeyboard();
                                brk2 = false;
                                brk = false;
                            break;

                            case b_A:
                                if ((chr != '\0' && chr >= '0' && chr <= '9') || chr == '\b') {
                                    switch (chr) {
                                        case '\b':
                                            valbuff[idx] /= 10;
                                            switch (Selection)
                                            {
                                                case 0:
                                                    if(dns_ip) POSCursor(33, 8);
                                                    else POSCursor(34, 8);
                                                break;

                                                case 1:
                                                    if(dns_ip) POSCursor(34, 10);
                                                    else POSCursor(36, 10);
                                                break;

                                                case 2:
                                                    POSCursor(32, 12);
                                                break;
                                            
                                                default:
                                                break;
                                            }
                                            switch (idx)
                                            {
                                                case 0:
                                                    printf("%s%d%s.%d.%d.%d        ", WHITE_BG_BLACK_FG, valbuff[0], DEFAULT_BG_FG, valbuff[1], valbuff[2], valbuff[3]);
                                                break;

                                                case 1:
                                                    printf("%d.%s%d%s.%d.%d        ", valbuff[0], WHITE_BG_BLACK_FG, valbuff[1], DEFAULT_BG_FG, valbuff[2], valbuff[3]);
                                                break;
                                                
                                                case 2:
                                                    printf("%d.%d.%s%d%s.%d        ", valbuff[0], valbuff[1], WHITE_BG_BLACK_FG, valbuff[2], DEFAULT_BG_FG, valbuff[3]);
                                                break;

                                                case 3:
                                                    printf("%d.%d.%d.%s%d%s        ", valbuff[0], valbuff[1], valbuff[2], WHITE_BG_BLACK_FG, valbuff[3], DEFAULT_BG_FG);
                                                break;
                                            
                                                default:
                                                break;
                                            }
                                            if (i > 0) i--;
                                        break;

                                        default:
                                            if((((valbuff[idx] * 10) + chr - 48) <= 255) && idx < 4){
                                                valbuff[idx] *= 10;
                                                valbuff[idx] += chr - 48;
                                                switch (Selection)
                                                {
                                                    case 0:
                                                        if(dns_ip) POSCursor(33, 8);
                                                        else POSCursor(34, 8);
                                                    break;

                                                    case 1:
                                                        if(dns_ip) POSCursor(34, 10);
                                                        else POSCursor(36, 10);
                                                    break;

                                                    case 2:
                                                        POSCursor(32, 12);
                                                    break;
                                                
                                                    default:
                                                    break;
                                                }
                                                if (i == 2) {
                                                    idx++;
                                                    if (idx == 4) {
                                                        for (size_t i = 0; i < 4; i++)
                                                        {
                                                            if (dns_ip) {
                                                                buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + i + (Selection * 4)] = valbuff[i];
                                                            } else {
                                                                buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + i + (Selection * 4)] = valbuff[i];
                                                            }
                                                        }
                                                        ClearKeyboard();
                                                        brk2 = false;
                                                        brk = false;
                                                    }
                                                    i = -1;
                                                }
                                                switch (idx)
                                                {
                                                    case 0:
                                                        printf("%s%d%s.%d.%d.%d        ", WHITE_BG_BLACK_FG, valbuff[0], DEFAULT_BG_FG, valbuff[1], valbuff[2], valbuff[3]);
                                                    break;

                                                    case 1:
                                                        printf("%d.%s%d%s.%d.%d        ", valbuff[0], WHITE_BG_BLACK_FG, valbuff[1], DEFAULT_BG_FG, valbuff[2], valbuff[3]);
                                                    break;
                                                    
                                                    case 2:
                                                        printf("%d.%d.%s%d%s.%d        ", valbuff[0], valbuff[1], WHITE_BG_BLACK_FG, valbuff[2], DEFAULT_BG_FG, valbuff[3]);
                                                    break;

                                                    case 3:
                                                        printf("%d.%d.%d.%s%d%s        ", valbuff[0], valbuff[1], valbuff[2], WHITE_BG_BLACK_FG, valbuff[3], DEFAULT_BG_FG);
                                                    break;
                                                
                                                    default:
                                                    break;
                                                }
                                                i++;
                                            }
                                        break;
                                    }
                                }
                            break;
                        
                            default:
                            break;
                        }
                    }
                break;

            }
        }
    }
}

void editwireless(int PROFNumber, uint8_t *buff) {
    int Selection = 0;
    while (1) {
        ClearScreen();

        POSCursor(30, 5);
        printf("WIFI Settings :");

        POSCursor(20, 8);
        printf("SSID : ");
        for (size_t i = (HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
            putchar(buff[i]);
        }

        POSCursor(20, 10);
        printf("PASSKEY : ");
        for (size_t i = (HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
            putchar(buff[i]);
        }

        POSCursor(20, 12);
        printf("ENCRYPTION : ");
        printf("%s", decodeencryption(buff[(HEADERSIZE + ENCRYPTIONTYPEOFFSET + ((PROFNumber - 1) * PROFSIZE))]));
        
        POSCursor(0, 26);
        printf("A : Edit\n");
        printf("HOME : Go back\n");

        POSCursor(18, 8 + (Selection * 2));
        printf("->");
        
        bool brk = true;

        while (brk) {
            int Input = CheckWPAD(0);

            switch (Input) {

                case HOME:
                    return;
                break;

                case DOWN:
                    if(Selection < 2) {
                        POSCursor(18, 8 + (Selection * 2));
                        printf("  ");
                        Selection++;
                        POSCursor(18, 8 + (Selection * 2));
                        printf("->");
                    }
                break;

                case UP:
                    if (Selection > 0) {
                        POSCursor(18, 8 + (Selection * 2));
                        printf("  ");
                        Selection--;
                        POSCursor(18, 8 + (Selection * 2));
                        printf("->");
                    }
                break;

                case b_A:
                    POSCursor(0, 26);
                    printf("\x1b[2K");
                    bool brk2 = true;
                    int idx = 0;
                    switch (Selection) {
                        case 0:
                            idx = buff[(HEADERSIZE + SSIDSIZEOFFSET + ((PROFNumber - 1) * PROFSIZE))];
                            POSCursor(27 + idx, 8);
                            
                            while (brk2) {
                                static bool shift = false;
                                static int conX, conY;
                                int Input = CheckWPAD(0);
                                WPADData* data = WPAD_Data(0);
                                int irX = (((data->ir.x) * 76) / 560);
                                int irY = (((data->ir.y) * 28) / 420);
                                CON_GetPosition(&conX, &conY);
                                char chr = keyboard(shift, irX, irY);
                                POSCursor(conX, conY);
                                VIDEO_WaitVSync();

                                switch (Input) {
                                    case b_B:
                                        if(shift) shift = false;
                                        else shift = true;
                                    break;

                                    case ONE:
                                        buff[(HEADERSIZE + SSIDSIZEOFFSET + ((PROFNumber - 1) * PROFSIZE))] = idx;
                                        ClearKeyboard();
                                        brk2 = false;
                                        brk = false;
                                    break;

                                    case b_A:
                                        if (chr != '\0' && idx < 32) {
                                            switch (chr)
                                            {
                                                case '\b':
                                                    if (idx > 0) {
                                                        printf("\b \b");
                                                        idx--;
                                                        buff[(HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                    }
                                                break;

                                                default:
                                                    putchar(chr);
                                                    buff[(HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    idx++;                                                    
                                                break;
                                            }
                                        }
                                    break;
                                
                                    default:
                                    break;
                                }
                            }
                        break;

                        case 1:
                            idx = buff[(HEADERSIZE + PASSKEYSIZEOFFSET + ((PROFNumber - 1) * PROFSIZE))];
                            POSCursor(30 + idx, 10);
                            
                            while (brk2) {
                                static bool shift = false;
                                static int conX, conY;
                                int Input = CheckWPAD(0);
                                WPADData* data = WPAD_Data(0);
                                int irX = (((data->ir.x) * 76) / 560);
                                int irY = (((data->ir.y) * 28) / 420);
                                CON_GetPosition(&conX, &conY);
                                char chr = keyboard(shift, irX, irY);
                                POSCursor(conX, conY);
                                VIDEO_WaitVSync();

                                switch (Input) {
                                    case b_B:
                                        if(shift) shift = false;
                                        else shift = true;
                                    break;

                                    case ONE:
                                        buff[(HEADERSIZE + PASSKEYSIZEOFFSET + ((PROFNumber - 1) * PROFSIZE))] = idx;
                                        ClearKeyboard();
                                        brk2 = false;
                                        brk = false;
                                    break;

                                    case b_A:
                                        if (chr != '\0' && idx < 32) {
                                            switch (chr)
                                            {
                                                case '\b':
                                                    if (idx > 0) {
                                                        printf("\b \b");
                                                        idx--;
                                                        buff[(HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = '\0';
                                                    }
                                                break;
                                                
                                                default:
                                                    putchar(chr);
                                                    buff[(HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE)) + idx] = chr;
                                                    idx++;                                                    
                                                break;
                                            }
                                        }
                                    break;
                                
                                    default:
                                    break;
                                }
                            }
                        break;

                        case 2:
                            buff[(HEADERSIZE + ENCRYPTIONTYPEOFFSET + ((PROFNumber - 1) * PROFSIZE))]++;
                            if (buff[(HEADERSIZE + ENCRYPTIONTYPEOFFSET + ((PROFNumber - 1) * PROFSIZE))] == 3) buff[(HEADERSIZE + ENCRYPTIONTYPEOFFSET + ((PROFNumber - 1) * PROFSIZE))] = 4;
                            if (buff[(HEADERSIZE + ENCRYPTIONTYPEOFFSET + ((PROFNumber - 1) * PROFSIZE))] == 7) buff[(HEADERSIZE + ENCRYPTIONTYPEOFFSET + ((PROFNumber - 1) * PROFSIZE))] = 0;

                            ClearKeyboard();
                            brk2 = false;
                            brk = false;
                        break;

                        default:
                        break;
                    }
                break;
            }
        }
    }    
}

void editprofile(int PROFNumber, uint8_t *cfgbuff, const char *cfgpath) {
    u8 aligned buff[7005];
    for (size_t i = 0; i < sizeof(buff); i++)
    {
        buff[i] = cfgbuff[i];
    }    
    ClearScreen();
    int Selection = 0;
    bool stayinloop = true;
    bool binary[8] = {false};
    Value2Binary(&buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE)], binary, true);
    while (stayinloop) {
        POSCursor(0, 0);
        printf("<Connection %d>\n", PROFNumber);
        printf("   Connection Selected? %s\n", binary[0] ? "Yes" : "No");
        printf("   Use PMTU Recovery? %s\n", binary[1] ? "Yes" : "No");
        printf("   Internet test passed? %s\n", binary[2] ? "Yes" : "No");
        printf("   Use Proxy? %s\n", binary[3] ? "Yes" : "No");
        if (binary[3]) {
            printf("     + Proxy Server : ");
            for (size_t i = (HEADERSIZE + PROXYSERVERNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                putchar(buff[i]);
            }
            printf("\n     + Port : %d", (buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET] * (0x10) * (0x10)) + buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE) + PROXYPORTOFFSET + 1]);
            if (buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1]) {
                printf("\n     + Username : ");
                for (size_t i = (HEADERSIZE + PROXYUSRNAMEOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                    putchar(buff[i]);
                }
                printf("\n     + Password : ");
                for (size_t i = (HEADERSIZE + PROXYPASSWORDOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                    putchar(buff[i]);
                }             
            }
            putchar('\n');
        }
        printf("   PPPoE? %s\n", binary[4] ? "Yes" : "No");
        printf("   DNS Source? %s\n", binary[5] ? "Auto" : "Manual");
        if (!binary[5]) {
            printf("     + Primary DNS : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE)], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 2], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 3]);
            printf("     + Secondary DNS : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 4], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 5], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 6], buff[HEADERSIZE + MANUALDNSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 7]);
        }
        printf("   IP source? %s\n", binary[6] ? "DHCP" : "Manual");
        if (!binary[6]) {
            printf("     + IP Address : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE)], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 2], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 3]);
            printf("     + Subnet Mask : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 4], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 5], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 6], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 7]);
            printf("     + Router IP : %d.%d.%d.%d\n", buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 8], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 9], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 10], buff[HEADERSIZE + MANUALIPOFFSET + ((PROFNumber - 1) * PROFSIZE) + 11]);
        }
        printf("   Interface? %s\n", binary[7] ? "Wired" : "Wireless");

        if (!binary[7]) {

            printf("\n   SSID : ");

            for (size_t i = (HEADERSIZE + SSIDOFFSET + ((PROFNumber - 1) * PROFSIZE)); buff[i] != '\0'; i++) {
                putchar(buff[i]);
            }
            
            printf("\n   PASSKEY : ");

            for (size_t i = 0; i < buff[HEADERSIZE + PASSKEYSIZEOFFSET + ((PROFNumber - 1) * PROFSIZE)]; i++) {
                putchar(buff[HEADERSIZE + PASSKEYOFFSET + ((PROFNumber - 1) * PROFSIZE) + i]);
            }

            printf("\n   ENCRYPTION : %s", decodeencryption(buff[HEADERSIZE + ENCRYPTIONTYPEOFFSET + (PROFNumber - 1) * PROFSIZE]));
        }

        POSCursor(0, 25);
        printf(" B : Go back without saving\n");
        printf(" PLUS : Go to submenu (Cursor needs to be : +>)\n");
        printf(" 1 : Save and go back");

        int offset = 0;

        if (Selection > 3) {
            offset += (binary[3] * (2 + (buff[HEADERSIZE + PROXYFLAGSOFFSET + ((PROFNumber - 1) * PROFSIZE) + 1] * 2)));
        }
        if (Selection > 5) {
            offset += (!binary[5]) ? 2 : 0;
        }
        if (Selection > 6) {
            offset += (!binary[6]) ? 3 : 0;
        }
        if (Selection > 7) {
            offset += (!binary[7]) ? 1 : 0;
        }

        POSCursor(0, 2 + Selection + offset);
        switch (Selection) {
            case 3:
            case 5:
            case 6:
            case 8:
                printf("+>");
            break;
        
            default:
                printf("->");
            break;
        }
        
        bool brk = true;
        while (brk) {
            int Input = CheckWPAD(0);

            switch (Input) {
                case DOWN:
                    if(Selection < 7 + (1 - binary[7])) {
                        Selection++;
                        brk = false;
                    }
                break;

                case UP:
                    if (Selection > 0) {
                        Selection--;
                        brk = false;
                    }
                break;

                case b_A:
                    if(Selection < 8) {
                        togglebit(binary, Selection);
                    }
                    brk = false;
                    ClearScreen();
                break;

                case ONE:
                    Value2Binary(&buff[HEADERSIZE + ((PROFNumber - 1) * PROFSIZE)], binary, false);
                    ISFS_Initialize();
                    s32 fcfg = ISFS_Open(cfgpath, ISFS_OPEN_RW);
                    int ret = ISFS_Write(fcfg, buff, sizeof(buff) - 1);
                    ClearScreen();
                    if (ret == sizeof(buff) - 1) printf("Success!");
                    else printf("Failed! %d", ret);
                    sleep(2);
                    ISFS_Close(fcfg);
                    ISFS_Deinitialize();
                case b_B:
                    ClearScreen();
                    printprofiledetails(PROFNumber, buff);
                    return;
                break;

                case PLUS:
                    switch (Selection) {
                        case 3:
                            editproxy(PROFNumber, buff);
                            brk = false;
                            ClearScreen();
                        break;

                        case 5:
                            editdns_ip(PROFNumber, buff, false);
                            brk = false;
                            ClearScreen();
                        break;

                        case 6:
                            editdns_ip(PROFNumber, buff, true);
                            brk = false;
                            ClearScreen();
                        break;

                        case 8:
                            editwireless(PROFNumber, buff);
                            brk = false;
                            ClearScreen();
                        break;
                    
                        default:
                        break;
                    }
                break;
            
                default:
                break;
            }
        }
    }
}