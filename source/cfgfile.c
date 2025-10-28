#include "cfgfile.h"
#include "WiiLibs.h"
#include "Video.h"
#include <fat.h>
#include <math.h>
#include "Input.h"
#include "virtualkb.h"

char *decodeencryption(char byte) {
    switch (byte) {
        case OPEN: 

            return "OPEN";
            
        break;

        case WEP64:

            return "WEP64";
            
        break;

        case WEP128: 

            return "WEP128";
            
        break;

        case WPA_PSK_TKIP: 

            return "WPA-PSK (TKIP)";
            
        break;

        case WPA2_PSK_AES: 

            return "WPA2-PSK (AES)";
            
        break;

        case WPA_PSK_AES: 

            return "WPA-PSK (AES)";
            
        break;
    
        default:
            return NULL;
        break;
    }
}

void dumpfile(netconfig_t *buff, size_t length, const char *path) {
    fatInitDefault();
    FILE* dmpf = fopen(path, "w+");
    if(!dmpf) return;

    fwrite(buff, length, sizeof(char), dmpf);

    fclose(dmpf);

    ClearScreen();

    printf("Successfully dumped!");
    sleep(2);
}

void printprofiledetails(int PROFNumber, connection_t *profile) {
    if (PROFNumber > 0 && PROFNumber < 4) {

        printf("<Connection %d %X>\n", PROFNumber, profile->flags);
        printf("Connection Selected? %s\n", (profile->flags & CONNECTION_SELECTED) ? "Yes" : "No");
        printf("Use PMTU Recovery? %s\n", (profile->flags & USE_PMTU) ? "Yes" : "No");
        printf("Internet test passed? %s\n", (profile->flags & TEST_PASSED) ? "Yes" : "No");
        printf("Use Proxy? %s\n", (profile->flags & USE_PROXY) ? "Yes" : "No");
        if (profile->flags & USE_PROXY) {
            printf("  + Proxy Server : ");
            
            printf("\n  + Port : %d", profile->proxy_settings.proxy_port);
            
            if (profile->proxy_settings.use_proxy_userandpass) {
                printf("\n  + Username : %s", profile->proxy_settings.proxy_username);
                printf("\n  + Password : %s", profile->proxy_settings.proxy_password);
            }
            
            putchar('\n'); 
        }
        printf("PPPoE? %s\n", (profile->flags & PPPOE) ? "Yes" : "No");
        printf("DNS Source? %s\n", (profile->flags & DNS) ? "Auto" : "Manual");
        if (!(profile->flags & DNS)) {
            printf("  + Primary DNS : %d.%d.%d.%d\n", profile->dns1[0], profile->dns1[1], profile->dns1[2], profile->dns1[3]);
            printf("  + Secondary DNS : %d.%d.%d.%d\n", profile->dns2[0], profile->dns2[1], profile->dns2[2], profile->dns2[3]);
        }
        printf("IP source? %s\n", (profile->flags & DHCP) ? "DHCP" : "Manual");
        if (!(profile->flags & DHCP)) {
            printf("  + IP Address : %d.%d.%d.%d\n", profile->ip[0], profile->ip[1], profile->ip[2], profile->ip[3]);
            printf("  + Subnet Mask : %d.%d.%d.%d\n", profile->netmask[0], profile->netmask[1], profile->netmask[2], profile->netmask[3]);
            printf("  + Router IP : %d.%d.%d.%d\n",profile->gateway[0], profile->gateway[1], profile->gateway[2], profile->gateway[3] );

        }
        printf("Interface? %s\n", (profile->flags & INTERFACE) ? "Wired" : "Wireless");

        if (!(profile->flags & INTERFACE)) {

            printf("\nSSID : %s", profile->ssid);
            printf("\nPASSKEY : %s", profile->key);
            printf("\nENCRYPTION : %s", decodeencryption(profile->encryption));
        }
    }

    POSCursor(0, 26);
    printf(" 1 : Dump config.dat\n");
    printf(" 2 : Edit current profile");
}



void editproxy(int PROFNumber, connection_t *profile) {
    if(profile->flags & 0x10) return;
    int Selection = 0;
    while (1) {
        ClearScreen();

        POSCursor(30, 4);
        printf("Proxy Settings :");

        POSCursor(20, 6);
        printf("Proxy Server : %s", profile->proxy_settings.proxy_name);


        POSCursor(20, 8);
        printf("Port : %d", profile->proxy_settings.proxy_port);

        POSCursor(20, 10);
        printf("Needs Username/Password? %s", profile->proxy_settings.use_proxy_userandpass ? "Yes" : "No");

        POSCursor(20, 12);
        printf("Username : %s", profile->proxy_settings.proxy_username);

        POSCursor(20, 14);
        printf("Password : %s", profile->proxy_settings.proxy_password);

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
                    if(profile->flags & 0x10) return;
                    
                    if(strlen((char*)profile->proxy_settings.proxy_name) > 0 && profile->proxy_settings.proxy_port > 0  && (strlen((char*)profile->proxy_settings.proxy_username) > 0 || !profile->proxy_settings.use_proxy_userandpass)) {
                        return;
                    }
                    POSCursor(14, 16);
                    if (strlen((char*)profile->proxy_settings.proxy_name) == 0) {
                        printf("Please specify a proxy name");
                    }
                    POSCursor(14, 17);
                    if (strlen((char*)profile->proxy_settings.proxy_username) == 0 && profile->proxy_settings.use_proxy_userandpass) {
                        printf("Please specify a proxy username");
                    }
                    POSCursor(14, 18);
                    if (profile->proxy_settings.proxy_port == 0) {
                        printf("Port can't be 0");
                    }
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
                            idx = strlen((char*)profile->proxy_settings.proxy_name);
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
                                                        profile->proxy_settings.proxy_name[idx] = '\0';
                                                        profile->proxy_settings_copy.proxy_name[idx] = '\0';
                                                    }
                                                case ' ':
                                                break;
                                            
                                                default:
                                                    putchar(chr);
                                                    profile->proxy_settings.proxy_name[idx] = chr;
                                                    profile->proxy_settings_copy.proxy_name[idx] = chr;
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

                                    case HOME:
                                        profile->proxy_settings.proxy_port = proxy;
                                        profile->proxy_settings_copy.proxy_port = proxy;
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
                            if (profile->proxy_settings.use_proxy_userandpass) {
                                profile->proxy_settings.use_proxy_userandpass = 0;
                                profile->proxy_settings_copy.use_proxy_userandpass = 0;
                            }
                            else {
                                profile->proxy_settings.use_proxy_userandpass = 1;
                                profile->proxy_settings_copy.use_proxy_userandpass = 1;
                            }

                            brk2 = false;
                            brk = false;
                        break;

                        case 3:
                            idx = strlen((char*)profile->proxy_settings.proxy_username);
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
                                                        profile->proxy_settings.proxy_username[idx] = '\0';
                                                        profile->proxy_settings_copy.proxy_username[idx] = '\0';
                                                    }
                                                case ' ':
                                                break;
                                            
                                                default:
                                                    putchar(chr);
                                                    profile->proxy_settings.proxy_username[idx] = chr;
                                                    profile->proxy_settings_copy.proxy_username[idx] = chr;
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
                            idx = strlen((char*)profile->proxy_settings.proxy_password);
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
                                                        profile->proxy_settings.proxy_password[idx] = '\0';
                                                        profile->proxy_settings_copy.proxy_password[idx] = '\0';
                                                    }
                                                case ' ':
                                                break;
                                            
                                                default:
                                                    putchar(chr);
                                                    profile->proxy_settings.proxy_password[idx] = chr;
                                                    profile->proxy_settings_copy.proxy_password[idx] = chr;
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

void editdns_ip(int PROFNumber, connection_t *profile, bool dns_ip) {
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
            printf("IP Address : %d.%d.%d.%d\n", profile->ip[0], profile->ip[1], profile->ip[2], profile->ip[3]);
        } else {
            printf("Primary DNS :  %d.%d.%d.%d\n", profile->dns1[0], profile->dns1[1], profile->dns1[2], profile->dns1[3]);
        }
        
        POSCursor(20, 10);
        if(dns_ip) {
            printf("Subnet Mask : %d.%d.%d.%d\n", profile->netmask[0], profile->netmask[1], profile->netmask[2], profile->netmask[3]);
        } else {
            printf("Secondary DNS : %d.%d.%d.%d\n", profile->dns2[0], profile->dns2[1], profile->dns2[2], profile->dns2[3]);
        }

        if (dns_ip) {
            POSCursor(20, 12);
            printf("Router IP : %d.%d.%d.%d", profile->gateway[0], profile->gateway[1], profile->gateway[2], profile->gateway[3]);
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
                                        profile->ip[i] = valbuff[i];
                                    } else {
                                       profile->dns1[i] = valbuff[i];
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
                                                                profile->ip[i] = valbuff[i];
                                                            } else {
                                                                profile->dns1[i] = valbuff[i];
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

void editwireless(int PROFNumber, connection_t *profile) {
    int Selection = 0;
    while (1) {
        ClearScreen();

        POSCursor(30, 5);
        printf("WIFI Settings :");

        POSCursor(20, 8);
        printf("SSID : %s", profile->ssid);

        POSCursor(20, 10);
        printf("PASSKEY : %s", profile->key);

        POSCursor(20, 12);
        printf("ENCRYPTION : %s",decodeencryption(profile->encryption));

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
                    POSCursor(14, 16);
                    if (profile->key_length == 0 && profile->encryption != OPEN) {
                        printf("Passkey can't be NULL!\n");
                        break;
                    }
                    if (profile->ssid_length == 0) {
                        printf("Please specify an SSID");
                        break;
                    }
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
                    if (Selection < 2) {
                        switch (Selection) {
                            case 0:
                                idx = profile->ssid_length;
                                POSCursor(27 + idx, 8);
                            break;

                            case 1:
                                if(profile->encryption == 0x00) {
                                    brk2 = false;
                                    brk = false;
                                } else {
                                    idx = profile->key_length;
                                    POSCursor(30 + idx, 10);
                                }
                            break;
                        }
                                
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
                                    switch (Selection) {
                                        case 0:
                                            profile->ssid_length = idx;
                                            ClearKeyboard();
                                            brk2 = false;
                                            brk = false;
                                        break;

                                        case 1:
                                            if((profile->encryption > 0x03 && idx > 7) || (profile->encryption == 0x01 && idx == 5) || (profile->encryption == 0x02 && idx == 13)) {
                                                profile->key_length = idx;
                                                ClearKeyboard();
                                                brk2 = false;
                                                brk = false;
                                            }
                                        break;
                                    }
                                break;

                                case b_A:
                                    if ((chr != '\0' && idx < 32) || chr == '\b') {
                                        switch (chr)
                                        {
                                            case '\b':
                                                if (idx > 0) {
                                                    printf("\b \b");
                                                    idx--;
                                                    switch (Selection) {
                                                        case 0:
                                                            profile->ssid[idx] = '\0';
                                                        break;

                                                        case 1:
                                                            profile->key[idx] = '\0';
                                                        break;
                                                    }
                                                }
                                            break;

                                            default:
                                                putchar(chr);
                                                switch (Selection) {
                                                    case 0:
                                                        profile->ssid[idx] = chr;
                                                    break;

                                                    case 1:
                                                        profile->key[idx] = chr;
                                                    break;
                                                }
                                                idx++;                                                    
                                            break;
                                        }
                                    }
                                break;
                            
                                default:
                                break;
                            }
                        }
                    
                    } else {
                        profile->encryption++;
                        if (profile->encryption  == 3) profile->encryption = 4;
                        if (profile->encryption  == 7) profile->encryption = 0;
                        for (size_t i = 0; i < profile->key_length; i++) {
                            profile->key[i] = '\0';
                        }
                        profile->key_length = '\0';
                        brk2 = false;
                        brk = false;
                    }
                
                break;
            }
        }
    }
}    


void editprofile(int PROFNumber, netconfig_t buff, const char *cfgpath) {
    ClearScreen();
    int Selection = 0;
    bool stayinloop = true;
    while (stayinloop) {
        POSCursor(0, 0);
        printf("<Connection %d %X>\n", PROFNumber, buff.connection[PROFNumber - 1].flags);
        printf("   Connection Selected? %s\n", (buff.connection[PROFNumber - 1].flags & CONNECTION_SELECTED) ? "Yes" : "No");
        printf("   Use PMTU Recovery? %s\n", (buff.connection[PROFNumber - 1].flags & USE_PMTU) ? "Yes" : "No");
        printf("   Internet test passed? %s\n", (buff.connection[PROFNumber - 1].flags & TEST_PASSED) ? "Yes" : "No");
        printf("   Use Proxy? %s\n", (buff.connection[PROFNumber - 1].flags & USE_PROXY) ? "Yes" : "No");
        if (buff.connection[PROFNumber - 1].flags & USE_PROXY) {
            printf("     + Proxy Server : ");
            
            printf("\n     + Port : %d", buff.connection[PROFNumber - 1].proxy_settings.proxy_port);
            
            if (buff.connection[PROFNumber - 1].proxy_settings.use_proxy_userandpass) {
                printf("\n     + Username : %s", buff.connection[PROFNumber - 1].proxy_settings.proxy_username);
                printf("\n     + Password : %s", buff.connection[PROFNumber - 1].proxy_settings.proxy_password);
            }
            
            putchar('\n'); 
        }
        printf("   PPPoE? %s\n", (buff.connection[PROFNumber - 1].flags & PPPOE) ? "Yes" : "No");
        printf("   DNS Source? %s\n", (buff.connection[PROFNumber - 1].flags & DNS) ? "Auto" : "Manual");
        if (!(buff.connection[PROFNumber - 1].flags & DNS)) {
            printf("     + Primary DNS : %d.%d.%d.%d\n", buff.connection[PROFNumber - 1].dns1[0], buff.connection[PROFNumber - 1].dns1[1], buff.connection[PROFNumber - 1].dns1[2], buff.connection[PROFNumber - 1].dns1[3]);
            printf("     + Secondary DNS : %d.%d.%d.%d\n", buff.connection[PROFNumber - 1].dns2[0], buff.connection[PROFNumber - 1].dns2[1], buff.connection[PROFNumber - 1].dns2[2], buff.connection[PROFNumber - 1].dns2[3]);
        }
        printf("   IP source? %s\n", (buff.connection[PROFNumber - 1].flags & DHCP) ? "DHCP" : "Manual");
        if (!(buff.connection[PROFNumber - 1].flags & DHCP)) {
            printf("     + IP Address : %d.%d.%d.%d\n", buff.connection[PROFNumber - 1].ip[0], buff.connection[PROFNumber - 1].ip[1], buff.connection[PROFNumber - 1].ip[2], buff.connection[PROFNumber - 1].ip[3]);
            printf("     + Subnet Mask : %d.%d.%d.%d\n", buff.connection[PROFNumber - 1].netmask[0], buff.connection[PROFNumber - 1].netmask[1], buff.connection[PROFNumber - 1].netmask[2], buff.connection[PROFNumber - 1].netmask[3]);
            printf("     + Router IP : %d.%d.%d.%d\n",buff.connection[PROFNumber - 1].gateway[0], buff.connection[PROFNumber - 1].gateway[1], buff.connection[PROFNumber - 1].gateway[2], buff.connection[PROFNumber - 1].gateway[3] );

        }
        printf("   Interface? %s\n", (buff.connection[PROFNumber - 1].flags & INTERFACE) ? "Wired" : "Wireless");

        if (!(buff.connection[PROFNumber - 1].flags & INTERFACE)) {

            printf("\n   SSID : %s", buff.connection[PROFNumber - 1].ssid);
            printf("\n   PASSKEY : %s", buff.connection[PROFNumber - 1].key);
            printf("\n   ENCRYPTION : %s", decodeencryption(buff.connection[PROFNumber - 1].encryption));
        }

        POSCursor(0, 25);
        printf(" B : Go back without saving\n");
        printf(" PLUS : Go to submenu (Cursor needs to be : +>)\n");
        printf(" 1 : Save and go back");

        int offset = 0;

        if (Selection > 3) {
            offset += ((buff.connection[PROFNumber - 1].flags & USE_PROXY)) ? 2 : 0;

            offset += ((buff.connection[PROFNumber - 1].proxy_settings.use_proxy_userandpass)) ? 2 : 0;
        }
        if (Selection > 5) {
            offset += (!(buff.connection[PROFNumber - 1].flags & DNS)) ? 2 : 0;
        }
        if (Selection > 6) {
            offset += (!(buff.connection[PROFNumber - 1].flags & DHCP)) ? 3 : 0;
        }
        if (Selection > 7) {
            offset += (!(buff.connection[PROFNumber - 1].flags & INTERFACE)) ? 1 : 0;
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
                    if(Selection < 7 + (1 - (buff.connection[PROFNumber - 1].flags & INTERFACE))) {
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
                        
                        
                    }
                    brk = false;
                    ClearScreen();
                break;

                case ONE:
                    ClearScreen();
                    if((buff.connection[PROFNumber - 1].flags & CONNECTION_SELECTED)) {
                        switch (PROFNumber)
                        {
                            case 1:
                                if(buff.connection[2].flags & 0x80) {
                                    buff.connection[2].flags -= 0x80;
                                    printf("Connection 3 was active; Disabling it\n");
                                }
                                if(buff.connection[1].flags & 0x80) {
                                    buff.connection[1].flags -= 0x80;
                                    printf("Connection 2 was active; Disabling it\n");
                                }
                            break;

                            case 2:
                                if(buff.connection[2].flags & 0x80) {
                                    buff.connection[2].flags -= 0x80;
                                    printf("Connection 3 was active; Disabling it\n");
                                }
                                if(buff.connection[0].flags & 0x80) {
                                    buff.connection[0].flags -= 0x80;
                                    printf("Connection 1 was active; Disabling it\n");
                                }
                            break;

                            case 3:
                                if(buff.connection[1].flags & 0x80) {
                                    buff.connection[1].flags -= 0x80;
                                    printf("Connection 2 was active; Disabling it\n");
                                }
                                if(buff.connection[0].flags & 0x80) {
                                    buff.connection[0].flags -= 0x80;
                                    printf("Connection 1 was active; Disabling it\n");
                                }
                            break;
                        
                        }
                    }
                    for (size_t i = 0; i < 2; i++)
                    {
                        if (buff.connection[i].flags & 0x20) buff.header4 = 1;
                    }
                    ISFS_Initialize();
                    s32 fcfg = ISFS_Open(cfgpath, ISFS_OPEN_RW);
                    int ret = ISFS_Write(fcfg, &buff, sizeof(buff) - 1);
                    if (ret == sizeof(buff) - 1) printf("Success!");
                    else printf("Failed! %d", ret);
                    sleep(2);
                    ISFS_Close(fcfg);
                    ISFS_Deinitialize();
                case b_B:
                    ClearScreen();
                    return;
                break;

                case PLUS:
                    switch (Selection) {
                        case 3:
                            editproxy(PROFNumber, &buff.connection[PROFNumber - 1]);
                            brk = false;
                            ClearScreen();
                        break;

                        case 5:
                            editdns_ip(PROFNumber, &buff.connection[PROFNumber - 1], false);
                            brk = false;
                            ClearScreen();
                        break;

                        case 6:
                            editdns_ip(PROFNumber, &buff.connection[PROFNumber - 1], true);
                            brk = false;
                            ClearScreen();
                        break;

                        case 8:
                            editwireless(PROFNumber, &buff.connection[PROFNumber - 1]);
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