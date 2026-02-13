#include "TUI.h"
#include "Input.h"
#include "virtualkb.h"
#include "Video.h"

void printtabletop() {
    putchar(201);
    for(int i = 0; i < 49; i++) {
        if(i == 5 || i == 38) {
            putchar(203);
        } else {
            putchar(205);
        }
    }
    putchar(187);
}

void printtablebottom() {
    putchar(200);
    for(int i = 0; i < 49; i++) {
        if(i == 5 || i == 38) {
            putchar(202);
        } else {
            putchar(205);
        }
    }
    putchar(188);
}

void ReadString(u8* dest, u8* len, u8 maxlen) {
    int idx = *len;
    bool shift = false;
    int conX, conY;
    while (1) {
        int Input = CheckWPAD(0);
        WPADData* data = WPAD_Data(0);
        CON_GetPosition(&conX, &conY);
        char chr = keyboard(shift, (((data->ir.x) * 76) / 560), (((data->ir.y) * 28) / 420));
        POSCursor(conX, conY);
        VIDEO_WaitVSync();

        switch (Input) {
            case b_B:
                if(shift) shift = false;
                else shift = true;
            break;

            case HOME:
                *len = idx;
                ClearKeyboard();
                return;
            break;

            case b_A:
                if ((chr != '\0' && idx < maxlen) || chr == '\b') {
                    switch (chr)
                    {
                        case '\b':
                            if (idx > 0) {
                                printf("\b \b");
                                idx--;
                                dest[idx] = '\0';
                            }
                        break;

                        default:
                            putchar(chr);
                            dest[idx] = chr;
                            idx++;                                                    
                        break;
                    }
                }
            break;
        
            default:
            break;
        }
    }
}

u32 ReadNumString(u32 maxval) {
    u32 val = 0;
    bool shift = false;
    int conX, conY;
    while (1) {
        int Input = CheckWPAD(0);
        WPADData* data = WPAD_Data(0);
        CON_GetPosition(&conX, &conY);
        char chr = numpad((((data->ir.x) * 76) / 560), (((data->ir.y) * 28) / 420));
        POSCursor(conX, conY);
        VIDEO_WaitVSync();

        switch (Input) {
            case b_B:
                if(shift) shift = false;
                else shift = true;
            break;

            case HOME:
                ClearKeyboard();
                return val;
            break;

            case b_A:
                if ((chr != '\0' && (chr > '0' - 1 && chr < '9' + 1) && val + (chr - '0') < maxval) || chr == '<') {
                    switch (chr)
                    {
                        case '<':
                            if (val > 0) {
                                val = val / 10; 
                                CON_GetPosition(&conX, &conY);
                                printf("%d     ", val);
                                POSCursor(conX, conY);
                            }
                        break;

                        default:
                            val = val * 10 + chr - '0';
                            CON_GetPosition(&conX, &conY);
                            printf("%d     ", val);
                            POSCursor(conX, conY);
                        break;
                    }
                }
            break;
        
            default:
            break;
        }
    }
}

void ReadDNS_IP(u8* dest) {
    u8 buff[4] = {0};
    u8 idx = 0;
    bool shift = false;
    int conX, conY;
    while (1) {
        int Input = CheckWPAD(0);
        WPADData* data = WPAD_Data(0);
        CON_GetPosition(&conX, &conY);
        char chr = numpad((((data->ir.x) * 76) / 560), (((data->ir.y) * 28) / 420));
        POSCursor(conX, conY);
        VIDEO_WaitVSync();

        switch (Input) {
            case b_B:
                if(shift) shift = false;
                else shift = true;
            break;

            case HOME:
                memcpy(dest, buff, 4);
                ClearKeyboard();
                return;
            break;

            case b_A:
                if (chr != '\0') {
                    switch (chr)
                    {
                        case '<':
                            if (buff[idx] > 0) {
                                buff[idx] = buff[idx] / 10; 
                            }
                        break;

                        case '.':
                            if(idx < 3) {
                                idx++;
                            } else {
                                memcpy(dest, buff, 4);
                                ClearKeyboard();
                                return;
                            }                            
                        break;

                        default:
                            if((buff[idx] * 10) + (chr - '0') < 255) {
                                buff[idx] = (buff[idx] * 10) + chr - '0';
                            } else {
                                idx++;
                                if(idx == 4) {
                                    memcpy(dest, buff, 4);
                                    ClearKeyboard();
                                    return;
                                }
                            }                      
                        break;
                    }
                }
                CON_GetPosition(&conX, &conY);
                for(u8 i = 0; i < 4; i++) {
                    if(i == idx) {
                        printf("%s", WHITE_BG_BLACK_FG);
                    }
                    printf("%d", buff[i]);
                    if(i == idx) {
                        printf("%s", DEFAULT_BG_FG);
                    }
                    if(i < 3) {
                        putchar('.');
                    }
                }
                printf("      ");
                POSCursor(conX, conY);
            break;
        
            default:
            break;
        }
    }
}