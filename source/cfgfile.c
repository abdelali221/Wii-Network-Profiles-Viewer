/*-----------------------------------------------------------------------

    Wii-Network-Profiles-Viewer --
        A network profiles viewer/editor for the Nintendo Wii

    https://github.com/abdelali221/Wii-Network-Profiles-Viewer

    Copyright (C) 2025 - 2026 Abdelali221

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

-----------------------------------------------------------------------*/

#include <fat.h>
#include "wd.h"
#include <math.h>
#include <unistd.h>

#include "cfgfile.h"
#include "WiiLibs.h"
#include "Video.h"
#include "Input.h"
#include "ncd.h"
#include "virtualkb.h"
#include "TUI.h"

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

        printf("<Connection %d>\n", PROFNumber);
        printf("Connection Selected? %s\n", (profile->flags & CONNECTION_SELECTED) ? "Yes" : "No");
        printf("Use PMTU Recovery? %s\n", (profile->flags & USE_PMTU) ? "Yes" : "No");
        printf("Internet test passed? %s\n", (profile->flags & TEST_PASSED) ? "Yes" : "No");
        printf("Use Proxy? %s\n", (profile->flags & USE_PROXY) ? "Yes" : "No");
        if (profile->flags & USE_PROXY) {
            printf("  + Proxy Server : %s", profile->proxy_settings.proxy_name);
            
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
            if(profile->encryption) {
                if(profile->encryption & (WEP64 | WEP128)) {
                    printf("\nPASSKEY : ");
                    if(profile->encryption == WEP64) {
                        if(profile->unknown) {
                            for(int i = 0; i < 5; i++) {
                                printf("%02X", profile->key[i]);
                            }
                            printf(" (HEX)");
                        } else {
                            for(int i = 0; i < 5; i++) {
                                putchar(profile->key[i]);
                            }
                        }
                    } else {
                        if(profile->unknown) {
                            for(int i = 0; i < 13; i++) {
                                printf("%02X", profile->key[i]);
                            }
                            printf(" (HEX)");
                        } else {
                            for(int i = 0; i < 13; i++) {
                                putchar(profile->key[i]);
                            }
                        }
                    }
                } else {
                    printf("\nPASSKEY : %s", profile->key);
                }
            }
            printf("\nENCRYPTION : %s", decodeencryption(profile->encryption));
        }
    }

    POSCursor(0, 25);
    printf(" 1 (X) : Dump config.dat\n");
    printf(" 2 (Y) : Edit current profile\n");
    printf(" PLUS (L) : Test connectivity of the current configuration");
}

void editproxy(int PROFNumber, connection_t *profile) {
    if(profile->flags & 0x10) return;
    int Selection = 0;
    while (1) {
        ClearScreen();

        POSCursor(30, 2);
        printf("Proxy Settings :");

        POSCursor(20, 4);
        printf("Proxy Server : %s", profile->proxy_settings.proxy_name);


        POSCursor(20, 6);
        printf("Port : %d", profile->proxy_settings.proxy_port);

        POSCursor(20, 8);
        printf("Needs Username/Password? %s", profile->proxy_settings.use_proxy_userandpass ? "Yes" : "No");

        POSCursor(20, 10);
        printf("Username : %s", profile->proxy_settings.proxy_username);

        POSCursor(20, 12);
        printf("Password : %s", profile->proxy_settings.proxy_password);

        POSCursor(0, 25);
        printf("A : Edit\n");
        printf("HOME (Start) : Go back\n");
        
        POSCursor(18, 4 + (Selection * 2));
        printf("->");
        
        bool brk = true;
        u8 idx = 0;

        while (brk) {
            int Input = CheckInput(0);

            switch (Input) {

                case HOME:
                    return;
                break;

                case DOWN:
                    if(Selection < 4) {
                        POSCursor(18, 4 + (Selection * 2));
                        printf("  ");
                        Selection++;
                        POSCursor(18, 4 + (Selection * 2));
                        printf("->");
                    }
                break;

                case UP:
                    if (Selection > 0) {
                        POSCursor(18, 4 + (Selection * 2));
                        printf("  ");
                        Selection--;
                        POSCursor(18, 4 + (Selection * 2));
                        printf("->");
                    }
                break;

                case b_A:
                    POSCursor(0, 25);
                    printf("\x1b[2K");
                    switch (Selection) {
                        case 0:
                            idx = strlen((char*)profile->proxy_settings.proxy_name);
                            POSCursor(35 + idx, 4);
                            ReadString(profile->proxy_settings.proxy_name, &idx, 32);
                            brk = false;
                        break;

                        case 1:
                            POSCursor(27, 6);
                            printf("     \b\b\b\b\b"); 
                            profile->proxy_settings.proxy_port = ReadNumString(65535);
                            brk = false;
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
                            brk = false;
                        break;

                        case 3:
                            idx = strlen((char*)profile->proxy_settings.proxy_username);
                            POSCursor(31 + idx, 10);
                            ReadString(profile->proxy_settings.proxy_username, &idx, 32);
                            brk = false;
                        break;

                        case 4:
                            idx = strlen((char*)profile->proxy_settings.proxy_password);
                            POSCursor(31 + idx, 12);
                            ReadString(profile->proxy_settings.proxy_password, &idx, 32);
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
            printf("Primary DNS : %d.%d.%d.%d\n", profile->dns1[0], profile->dns1[1], profile->dns1[2], profile->dns1[3]);
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

        POSCursor(0, 25);
        printf("A : Edit\n");
        printf("HOME (Start) : Go back\n");

        POSCursor(18, 8 + (Selection * 2));
        printf("->");
        bool brk = true;
        
        while (brk) {
            int Input = CheckInput(0);
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
                    switch (Selection)
                    {
                        case 0:
                            if(dns_ip) {
                                POSCursor(33, 8);
                                ReadDNS_IP(profile->ip);
                            } else {
                                POSCursor(34, 8);
                                ReadDNS_IP(profile->dns1);
                            }
        
                        break;

                        case 1:
                            if(dns_ip) {
                                POSCursor(34, 10);
                                ReadDNS_IP(profile->netmask);
                            } else {
                                POSCursor(36, 10);
                                ReadDNS_IP(profile->dns2);
                            }
                        break;

                        case 2:
                            POSCursor(32, 12);
                            ReadDNS_IP(profile->gateway);
                        break;
                    
                        default:
                        break;
                    }
                    brk = false;
                break;

            }
        }
    }
}

bool isHexOnly(char* str, u8 size) {
    for(int i = 0;i < size - 1;i++) {
        if(!((str[i] >= '0' && str[i] <= '9') ||
         (str[i] >= 'A' && str[i] <= 'F')))  {
            return false;
         }
    }
    return true;
}

u8 chartohex(char c) {
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'A' && c <= 'F') return c - 'A' + 0xA;
    return 0;
}

char hextochar(u8 v) {
    if(v >= 0x0 && v <= 0x9) return v + '0';
    if(v >= 0xA && v <= 0xF) return v + 'A' - 0xA;
    return 0;
}

void ConvertASCIIStringToHex(u8* dest, char* src, u8 len) {
    for(int j = 0; j < (len/2); j++) {
        dest[j] = (chartohex(src[j*2]) << 4) | chartohex(src[(j*2) + 1]);
    }
}

void ConvertHexToASCIISrting(char* dest, u8* src, u8 len) {
    for(int j = 0; j < (len/2); j++) {
        dest[j*2] = (hextochar(src[j] >> 4));
        dest[j*2 + 1] = (hextochar(src[j] & 0xF));
    }
}

void editwireless(int PROFNumber, connection_t *profile) {
    int Selection = 0;
    while (1) {
printmenu:
        ClearScreen();

        POSCursor(30, 5);
        printf("WIFI Settings :");

        POSCursor(20, 8);
        printf("SSID : %s", profile->ssid);

        POSCursor(20, 10);
        if(profile->encryption) {
            if(profile->encryption == WEP64 || profile->encryption == WEP128) {
                printf("PASSKEY : ");
                if(profile->encryption == WEP64) {
                    if(profile->unknown) {
                        for(int i = 0; i < 5; i++) {
                            printf("%02X", profile->key[i]);
                        }
                    } else {
                        for(int i = 0; i < 5; i++) {
                            putchar(profile->key[i]);
                        }
                    }
                } else {
                    if(profile->unknown) {
                        for(int i = 0; i < 13; i++) {
                            printf("%02X", profile->key[i]);
                        }
                    } else {
                        for(int i = 0; i < 13; i++) {
                            putchar(profile->key[i]);
                        }
                    }
                }
            } else {
                printf("PASSKEY : %s", profile->key);
            }
        }

        POSCursor(20, 12);
        printf("ENCRYPTION : %s",decodeencryption(profile->encryption));

        POSCursor(0, 25);
        printf("A : Edit\n");
        printf("HOME (Start) : Go back\n");

        POSCursor(18, 8 + (Selection * 2));
        printf("->");
        
        bool brk = true;

        while (brk) {
            int Input = CheckInput(0);

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
                    POSCursor(0, 25);
                    printf("\x1b[2K");
                    u8 idx = 0;
                    if (Selection < 2) {
                        switch (Selection) {
                            case 0:
                                idx = profile->ssid_length;
                                POSCursor(27 + idx, 8);
                                ReadString(profile->ssid, &profile->ssid_length, 32);
                            break;

                            case 1:
                                if(profile->encryption == 0x00) {
                                    brk = false;
                                } else {
                                    idx = profile->key_length;
                                    u8* str = calloc(64, 1);
                                    if (profile->encryption == WEP64) {
                                        if(profile->unknown) {
                                            idx = 10;
                                            ConvertHexToASCIISrting((char*)str, profile->key, 10);
                                        } else {
                                            memcpy(str, profile->key, 5);
                                            idx = 5;
                                        }
                                    } else if(profile->encryption == WEP128) {
                                        if(profile->unknown) {
                                            idx = 26;
                                            ConvertHexToASCIISrting((char*)str, profile->key, 13);
                                        } else {
                                            memcpy(str, profile->key, 13);
                                            idx = 13;
                                        }
                                    } else {
                                        memcpy(str, profile->key, idx);
                                    }
                                    POSCursor(30 + idx, 10);
                                    ReadString(str, &idx, 64);
                                    if(profile->encryption > 3) {
                                        if(idx < 8) {
                                            memset(profile->key, 0, 64);
                                            memset(profile->key, '0', 8);
                                            profile->key_length = 8;
                                        } else {
                                            memcpy(profile->key, str, 64);
                                            profile->key_length = idx;
                                        }
                                    } else if (profile->encryption == WEP64) {
                                        if(idx == 10 && isHexOnly((char*)str, 10)) {
                                            ConvertASCIIStringToHex(profile->key, (char*)str, 10);
                                            for(int i = 0; i < 3; i++) {
                                                memcpy(profile->key + 5 + (i*5), profile->key, 5);
                                            }
                                            profile->unknown = 1;
                                        } else if(idx == 5) {
                                            profile->unknown = 0;
                                            for(int i = 0; i < 4; i++) {
                                                memcpy(profile->key+(i*5), str, 5);
                                            }
                                        } else {
                                            profile->unknown = 0;
                                            memset(profile->key, '0', 20);
                                        }
                                        profile->key_length = 0;
                                    } else if(profile->encryption == WEP128) {
                                        if(idx == 26 && isHexOnly((char*)str, 26)) {
                                            ConvertASCIIStringToHex(profile->key, (char*)str, 26);
                                            for(int i = 0; i < 3; i++) {
                                                memcpy(profile->key + 13 + (i*13), profile->key, 13);
                                            }
                                            profile->unknown = 1;
                                        } else if(idx == 13) {
                                            profile->unknown = 0;
                                            for(int i = 0; i < 4; i++) {
                                                memcpy(profile->key+(i*13), str, 13);
                                            }
                                        } else {
                                            profile->unknown = 0;
                                            memset(profile->key, '0', 52);
                                        }
                                        profile->key_length = 0;
                                    }
                                    free(str);
                                }
                                goto printmenu;
                            break;
                        }           
                    } else {
                        profile->encryption++;
                        profile->unknown = 0;
                        switch(profile->encryption) {
                            case WEP64:
                                memset(profile->key, '0', 20);
                                profile->key_length = 0;
                            break;

                            case WEP128:
                                memset(profile->key, '0', 52);
                                profile->key_length = 0;
                            break;

                            case 3:
                                profile->encryption = 4;
                                memset(profile->key, 0, 64);
                                memset(profile->key, '0', 8);
                                profile->key_length = 8;
                            break;

                            case WPA_PSK_TKIP:
                                profile->unknown = 1;
                            break;

                            case WPA2_PSK_AES:
                                profile->unknown = 0;
                            break;

                            case 7:
                                profile->encryption = 0;
                                memset(profile->key, 0, 64);
                                profile->key_length = 0;
                            break;
                            
                            default:
                            break;
                        }
                        brk = false;
                    }
                POSCursor(0, 25);
                printf("A : Edit\n");
                printf("HOME (Start) : Go back\n");
                break;
            }
        }
    }
}

BSSDescriptor* ParseScanBuff(u8 *ScanBuff, u8 X, u8 Y, u8 AP) {
    u16 APs = ScanBuff[0] << 8 | ScanBuff[1];
    BSSDescriptor* ptr = (BSSDescriptor*)((u32)ScanBuff + 2);
    BSSDescriptor* ret = (BSSDescriptor*)((u32)ScanBuff + 2);;

    POSCursor((X/2) - 5, 9);
    if(APs) {
        printf("Found %d APs", APs);
    } else {
        printf("No APs were found.");
    }

    POSCursor(X/4 - 4, 11);
    printtabletop();
    size_t i = 0;
    
    for (; i < APs; i++)
    {
        POSCursor(X/4 - 4, 12 + i);
        putchar(186);

        switch(WD_GetRadioLevel(ptr)) {
            case WD_SIGNAL_STRONG:
                printf("[###]");
            break;

            case WD_SIGNAL_NORMAL:
                printf("[## ]");
            break;

            case WD_SIGNAL_FAIR:
                printf("[#  ]");
            break;

            case WD_SIGNAL_WEAK:
                printf("[   ]");
            break;
        }

        putchar(186);

        POSCursor(X/4 + 1 + 16 - (ptr->SSIDLength / 2), 12 + i);
        if(i == AP) {
            ret = ptr;
            printf("%s%s%s%s%s", CONSOLE_BLACK, CONSOLE_BG_WHITE, ptr->SSID, CONSOLE_WHITE, CONSOLE_BG_BLACK);
        } else {
            printf("%s", ptr->SSID);
        }

        POSCursor(X/4 + 35, 12 + i);
        putchar(186);

        u8 Security = WD_GetSecurity(ptr);
        
        if(Security & WD_WPA2_AES) {
            printf(" WPA2-AES");
        } else if(Security & WD_WPA_AES) {
            printf("  WPA-AES");
        } else if(Security & WD_WPA_TKIP) {
            printf(" WPA-TKIP");
        } else if(Security & WD_WEP) {
            printf("   WEP");
        } else if(Security == 0) {
            printf("   Open");
        } else {
            printf(" Unknown");
        }
  
        if (ptr->length == 0) {
            if((ptr->IEs_length + 0x3E) % 2 == 0) {
                ptr = (BSSDescriptor*)((u32)ptr + ptr->IEs_length + 0x3E);
            } else {
                ptr = (BSSDescriptor*)((u32)ptr + ptr->IEs_length + 0x3F);
            }            
        } else {
            ptr = (BSSDescriptor*)((u32)ptr + ptr->length*2);
        }
        POSCursor(X/4 + 46, 12 + i);
        putchar(186);
    }
    
    POSCursor(X/4 - 4, 12 + i);
    printtablebottom();

    return ret;
}

void ScanWiFi(int PROFNumber, connection_t *profile) {
    ClearScreen();
    int X = 0, Y = 0;
    CON_GetMetrics(&X, &Y);
    POSCursor((X/2) - 8, 2);
    printf("WiFi Scan Wizard");

    ScanParameters set;
    WD_SetDefaultScanParameters(&set);

    WDInfo inf;
    WD_GetInfo(&inf);

    POSCursor((X/2) - 16, 7);
    printf("Scanning for WiFi networks nearby...");
    
    set.ChannelBitmap = inf.EnableChannelsMask;

    u8 ScanBuff[4096];
    WD_ScanOnce(&set, ScanBuff, sizeof(ScanBuff));
    u16 APs = ScanBuff[0] << 8 | ScanBuff[1];

    POSCursor(0, 24);
    printf(" B : Go back");
    if(APs > 0) printf("\n A : Select AP");
    printf("\n 2 (Y) : Scan Again");

    int AP = 0;
    BSSDescriptor* ptr = ParseScanBuff(ScanBuff, X, Y, AP);

    while (1) {
        int Input = CheckInput(0);
        switch(Input) {
            case UP:
                if(AP > 0) {
                    AP--;
                    ptr = ParseScanBuff(ScanBuff, X, Y, AP);
                } else {
                    AP = APs - 1;
                    ptr = ParseScanBuff(ScanBuff, X, Y, AP);
                }
            break;

            case DOWN:
                if(AP < APs - 1) {
                    AP++;
                    ptr = ParseScanBuff(ScanBuff, X, Y, AP);
                } else {
                    AP = 0;
                    ptr = ParseScanBuff(ScanBuff, X, Y, AP);
                }
            break;

            case TWO:

                ClearScreen();
                POSCursor((X/2) - 8, 2);
                printf("WiFi Scan Wizard");

                POSCursor((X/2) - 16, 7);
                printf("Scanning for WiFi networks nearby...");

                WD_ScanOnce(&set, ScanBuff, sizeof(ScanBuff));
                APs = ScanBuff[0] << 8 | ScanBuff[1];
                AP = 0;
                ptr = ParseScanBuff(ScanBuff, X, Y, AP);
                POSCursor(0, 24);
                printf(" B : Go back");
                if(APs > 0) printf("\n A : Select AP");
                printf("\n 2 (Y) : Scan Again");
            break;

            case b_A:
                u8 Security = WD_GetSecurity(ptr);

                if(Security != WD_OPEN && APs) {
                    ClearScreen();
                    POSCursor((X/2) - 8, 2);
                    printf("WiFi Scan Wizard");

                    POSCursor(0, 26);
                    printf(" Home : Save | 1 : Cancel");

                    POSCursor((X/2) - 15, 7);
                    printf("Please enter the password : ");
                    u8 keybuff[64];
                    memset(keybuff, 0, 64);
                    u8 keylen;
                    bool brk = 0;
                    while(!brk) {
                        POSCursor((X/2) - 13, 9);
                        printf("-> %*c", 32, ' ');
                        POSCursor((X/2) - 10, 9);
                        keylen = 0;
                        int ret = ReadString(keybuff, &keylen, 64);
                        if(ret) return;
                        if(Security & WD_WPA2_AES) {
                            if(keylen < 8) {
                                POSCursor((X/2) - 9, 11);
                                printf(" Password is invalid!");
                            } else {
                                profile->encryption = WPA2_PSK_AES;
                                profile->unknown = 0;
                                brk = 1;
                            }
                        } else if (Security & WD_WPA_AES) {
                            if(keylen < 8) {
                                POSCursor((X/2) - 9, 11);
                                printf(" Password is invalid!");
                            } else {
                                profile->encryption = WPA_PSK_AES;
                                profile->unknown = 0;
                                brk = 1;
                            }
                        } else if (Security & WD_WPA_TKIP) {
                            if(keylen < 8) {
                                POSCursor((X/2) - 9, 11);
                                printf(" Password is invalid!");
                            } else {
                                profile->encryption = WPA_PSK_TKIP;
                                profile->unknown = 1;
                                brk = 1;
                            }
                        } else if (Security & WD_WEP) {
                            if(keylen == 5) {
                                profile->encryption = WEP64;
                                profile->unknown = 0;
                                brk = 1;
                            } else if(keylen == 13) {
                                profile->encryption = WEP128;
                                profile->unknown = 0;
                                brk = 1;
                            } else if(keylen == 10 && isHexOnly((char*)keybuff, 10)) {
                                profile->encryption = WEP64;
                                profile->unknown = 1;
                                brk = 1;
                            } else if(keylen == 26 && isHexOnly((char*)keybuff, 26)) {
                                profile->encryption = WEP128;
                                profile->unknown = 1;
                                brk = 1;
                            } else {
                                POSCursor((X/2) - 9, 11);
                                printf(" Password is invalid!");
                            }
                        }
                        sleep(1);
                        POSCursor((X/2) - 9, 11);
                        printf("%*c", 21, ' ');
                        POSCursor(0, 26);
                        printf(" Home : Save | 1 : Cancel");
                    }
                    strcpy((char*)profile->ssid, (char*)ptr->SSID);
                    profile->ssid_length = ptr->SSIDLength;
                    memset(profile->key, 0, 64);
                    if(Security & WD_WEP) {
                        if(profile->unknown) {
                            ConvertASCIIStringToHex(profile->key, (char*)keybuff, keylen);
                        } else {
                            for(int i = 0; i < 4; i++) {
                                strncpy((char*)profile->key + (keylen*i),(char*)keybuff, keylen);
                            }
                        }                   
                    } else {
                        profile->key_length = keylen;
                        strcpy((char*)profile->key,(char*)keybuff);
                    }
                    return;
                } else if(Security == 0 && APs) {
                    strcpy((char*)profile->ssid, (char*)ptr->SSID);
                    memset(profile->key, 0, 64);
                    profile->ssid_length = ptr->SSIDLength;
                    profile->encryption = WD_OPEN;
                    return;
                }
            break;

            case b_B:
                return;
            break;
        }
    }
}

void editprofile(int PROFNumber, netconfig_t* origbuff) {
    netconfig_t aligned buff;
    memcpy(&buff, origbuff, sizeof(netconfig_t));
    ClearScreen();
    int Selection = 0;
    bool stayinloop = true;
    while (stayinloop) {
        POSCursor(0, 0);
        printf("<Connection %d>\n", PROFNumber);
        printf("   Connection Selected? %s\n", (buff.connection[PROFNumber - 1].flags & CONNECTION_SELECTED) ? "Yes" : "No");
        printf("   Use PMTU Recovery? %s\n", (buff.connection[PROFNumber - 1].flags & USE_PMTU) ? "Yes" : "No");
        printf("   Internet test passed? %s\n", (buff.connection[PROFNumber - 1].flags & TEST_PASSED) ? "Yes" : "No");
        printf("   Use Proxy? %s\n", (buff.connection[PROFNumber - 1].flags & USE_PROXY) ? "Yes" : "No");
        if (buff.connection[PROFNumber - 1].flags & USE_PROXY) {
            printf("     + Proxy Server : %s", buff.connection[PROFNumber - 1].proxy_settings.proxy_name);
            
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
            if(buff.connection[PROFNumber - 1].encryption) {
                if(buff.connection[PROFNumber - 1].encryption == WEP64 ||
                    buff.connection[PROFNumber - 1].encryption == WEP128) {
                    printf("\n   PASSKEY : ");
                    if(buff.connection[PROFNumber - 1].encryption == WEP64) {
                        if(buff.connection[PROFNumber - 1].unknown) {
                            for(int i = 0; i < 5; i++) {
                                printf("%02X", buff.connection[PROFNumber - 1].key[i]);
                            }
                            printf(" (HEX)");
                        } else {
                            for(int i = 0; i < 5; i++) {
                                putchar(buff.connection[PROFNumber - 1].key[i]);
                            }
                        }
                    } else {
                        if(buff.connection[PROFNumber - 1].unknown) {
                            for(int i = 0; i < 13; i++) {
                                printf("%02X", buff.connection[PROFNumber - 1].key[i]);
                            }
                            printf(" (HEX)");
                        } else {
                            for(int i = 0; i < 13; i++) {
                                putchar(buff.connection[PROFNumber - 1].key[i]);
                            }
                        }
                    }
                } else {
                    printf("\n   PASSKEY : %s", buff.connection[PROFNumber - 1].key);
                }
            }
            printf("\n   ENCRYPTION : %s", decodeencryption(buff.connection[PROFNumber - 1].encryption));
        }

        if(!(buff.connection[PROFNumber - 1].flags & INTERFACE)) {
            POSCursor(0, 23);
        } else {
            POSCursor(0, 24);
        }
        
        printf(" B : Go back without saving\n");
        printf(" PLUS (L) : Go to submenu (Cursor needs to be : +>)\n");
        printf(" MINUS (R) : Clear Profile\n");
        printf(" 1 (X) : Save and go back");
        if(!(buff.connection[PROFNumber - 1].flags & INTERFACE)) {
            printf("\n 2 (Y) : Scan for AP");
        }

        int offset = 0;

        if (Selection > 3) {
            offset += ((buff.connection[PROFNumber - 1].flags & USE_PROXY)) ? 2 : 0;
            if(buff.connection[PROFNumber - 1].flags & USE_PROXY) {
                offset += ((buff.connection[PROFNumber - 1].proxy_settings.use_proxy_userandpass)) ? 2 : 0;
            }
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
            int Input = CheckInput(0);

            switch (Input) {
                case DOWN:
                    if(Selection < 7 + (1 - (buff.connection[PROFNumber - 1].flags & INTERFACE))) {
                        Selection++;
                        brk = false;
                    } else {
                        Selection = 0;
                        brk = false;
                    }
                break;

                case UP:
                    if (Selection > 0) {
                        Selection--;
                        brk = false;
                    } else {
                        Selection = 7 + (1 - (buff.connection[PROFNumber - 1].flags & INTERFACE));
                        brk = false;
                    }
                break;

                case b_A:
                    if(Selection < 8) {
                        switch (Selection)
                        {
                            case 0:
                                if(buff.connection[PROFNumber - 1].flags & CONNECTION_SELECTED)
                                  buff.connection[PROFNumber - 1].flags -= CONNECTION_SELECTED;
                                else buff.connection[PROFNumber - 1].flags += CONNECTION_SELECTED;
                            break;

                            case 1:
                                if(buff.connection[PROFNumber - 1].flags & USE_PMTU)
                                  buff.connection[PROFNumber - 1].flags -= USE_PMTU;
                                else buff.connection[PROFNumber - 1].flags += USE_PMTU;
                            break;

                            case 2:
                                if(buff.connection[PROFNumber - 1].flags & TEST_PASSED)
                                  buff.connection[PROFNumber - 1].flags -= TEST_PASSED;
                                else buff.connection[PROFNumber - 1].flags += TEST_PASSED;
                            break;

                            case 3:
                                if(buff.connection[PROFNumber - 1].flags & USE_PROXY)
                                  buff.connection[PROFNumber - 1].flags -= USE_PROXY;
                                else buff.connection[PROFNumber - 1].flags += USE_PROXY;
                            break;

                            case 4:
                                if(buff.connection[PROFNumber - 1].flags & PPPOE)
                                  buff.connection[PROFNumber - 1].flags -= PPPOE;
                                else buff.connection[PROFNumber - 1].flags += PPPOE;
                            break;

                            case 5:
                                if(buff.connection[PROFNumber - 1].flags & DNS)
                                  buff.connection[PROFNumber - 1].flags -= DNS;
                                else buff.connection[PROFNumber - 1].flags += DNS;
                            break;

                            case 6:
                                if(buff.connection[PROFNumber - 1].flags & DHCP)
                                  buff.connection[PROFNumber - 1].flags -= DHCP;
                                else buff.connection[PROFNumber - 1].flags += DHCP;
                            break;

                            case 7:
                                if(buff.connection[PROFNumber - 1].flags & INTERFACE)
                                  buff.connection[PROFNumber - 1].flags -= INTERFACE;
                                else buff.connection[PROFNumber - 1].flags += INTERFACE;
                            break;
                        
                        default:
                            break;
                        }
                        
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
                                if(buff.connection[2].flags & CONNECTION_SELECTED) {
                                    buff.connection[2].flags -= 0x80;
                                    printf("Connection 3 was active; Disabling it\n");
                                }
                                if(buff.connection[1].flags & CONNECTION_SELECTED) {
                                    buff.connection[1].flags -= 0x80;
                                    printf("Connection 2 was active; Disabling it\n");
                                }
                            break;

                            case 2:
                                if(buff.connection[2].flags & CONNECTION_SELECTED) {
                                    buff.connection[2].flags -= 0x80;
                                    printf("Connection 3 was active; Disabling it\n");
                                }
                                if(buff.connection[0].flags & CONNECTION_SELECTED) {
                                    buff.connection[0].flags -= 0x80;
                                    printf("Connection 1 was active; Disabling it\n");
                                }
                            break;

                            case 3:
                                if(buff.connection[1].flags & CONNECTION_SELECTED) {
                                    buff.connection[1].flags -= 0x80;
                                    printf("Connection 2 was active; Disabling it\n");
                                }
                                if(buff.connection[0].flags & CONNECTION_SELECTED) {
                                    buff.connection[0].flags -= 0x80;
                                    printf("Connection 1 was active; Disabling it\n");
                                }
                            break;
                        }
                    }
                    buff.header4 = 0;
                    for (size_t i = 0; i < 3; i++)
                    {
                        if(buff.connection[i].flags & TEST_PASSED &&
                           buff.connection[i].flags & CONNECTION_SELECTED)
                           buff.header4 = 1;
                    }
                    int ret = NCD_WriteConfig(&buff);
                    NCD_Status status = NCD_GetStatus();
                    if (ret < 0 || status < 0) {
                        printf("Failed! Error %d", status);
                        sleep(3);
                        brk = false;
                        ClearScreen();
                        break;
                    }
                    else printf("Success!");
                    sleep(2);
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

                case MINUS:
                    memset(&buff.connection[PROFNumber - 1], 0, sizeof(connection_t));
                    brk = false;
                    ClearScreen();
                break;

                case TWO:
                    ScanWiFi(PROFNumber, &buff.connection[PROFNumber - 1]);
                    brk = false;
                    ClearScreen();
                break;
            
                default:
                break;
            }
        }
    }
}