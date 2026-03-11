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
    bool use_dpad = false;
    int conX = 0, conY = 0, x = 0, y = 0, irX, irY;
    while (1) {
        int Input = CheckInput(0);
        WPADData* data = WPAD_Data(0);
        irX = ((data->ir.x) * 76) / 560;
        irY = ((data->ir.y) * 28) / 420;

        if(!use_dpad) {
            x = irX;
            y = irY;
        }
        CON_GetPosition(&conX, &conY);
        char chr = keyboard(shift, x, y);

        if(!keyboard_isIRinrange(irX, irY) && !use_dpad) {
            x = KEYBOARD_X;
            y = KEYBOARD_Y;
        }

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
        
            case UP:
                if(!keyboard_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(y > (KEYBOARD_Y - 2)) y -= 2;
                }
            break;

            case DOWN:
                if(!keyboard_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if((y < (KEYBOARD_Y - 2) + 2 * 4 && (x < KEYBOARD_X + 11 * 4)) ||
                       (y < (KEYBOARD_Y - 2) + 2 * 2 && (x == KEYBOARD_X + 11 * 4)) ) y += 2;
                }
            break;

            case LEFT:
                if(!keyboard_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(x > KEYBOARD_X && y < (KEYBOARD_Y - 2) + 2 * 4) x -= 4;
                    else if ((x > KEYBOARD_X + 7 * 4) && (y == (KEYBOARD_Y - 2) + 2 * 4)) x = KEYBOARD_X + 6 * 4;
                    else if ((x > KEYBOARD_X) && (y == (KEYBOARD_Y - 2) + 2 * 4)) x = KEYBOARD_X + 8 * 4;
                }
            break;

            case RIGHT:
                if(!keyboard_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if((((x < KEYBOARD_X + 11 * 4) && (y < (KEYBOARD_Y - 2) + 2 * 3))) ||
                        ((x < KEYBOARD_X + 10 * 4) && (y < (KEYBOARD_Y - 2) + 2 * 4))) x += 4;
                    else if ((x < KEYBOARD_X + 8 * 4) && (y == (KEYBOARD_Y - 2) + 2 * 4)) x = KEYBOARD_X + 8 * 4;
                    else if ((x < KEYBOARD_X + 11 * 4) && (y == (KEYBOARD_Y - 2) + 2 * 4)) x = KEYBOARD_X + 6 * 4;
                }                
            break;
        
            default:
            break;
        }
        if(keyboard_isIRinrange(irX, irY)) {
            use_dpad = false;
        }
    }
}

u32 ReadNumString(u32 maxval) {
    u32 val = 0;
    bool shift = false;
    bool use_dpad = false;
    int conX, conY, x = 0, y = 0, irX, irY;
    while (1) {
        int Input = CheckInput(0);
        WPADData* data = WPAD_Data(0);
        irX = ((data->ir.x) * 76) / 560;
        irY = ((data->ir.y) * 28) / 420;

        if(!use_dpad) {
            x = irX;
            y = irY;
        }

        CON_GetPosition(&conX, &conY);
        char chr = numpad(x, y);

        if(!numpad_isIRinrange(irX, irY) && !use_dpad) {
            x = NUMPAD_X - 2;
            y = NUMPAD_Y - 2;
        }
        
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

            case UP:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(y > (NUMPAD_Y - 2)) y -= 2;
                }
            break;

            case DOWN:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(y < (NUMPAD_Y - 2) + 2 * 3) y += 2;
                }
            break;

            case LEFT:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(x > NUMPAD_X + 4) x -= 4;
                }
            break;

            case RIGHT:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(x < NUMPAD_X + 4 * 2) x += 4;
                }                
            break;
        
            default:
            break;
        }
        if(numpad_isIRinrange(irX, irY)) {
            use_dpad = false;
        }
    }
}

void ReadDNS_IP(u8* dest) {
    u8 buff[4] = {0};
    u8 idx = 0;
    bool shift = false;
    bool use_dpad = false;
    int conX, conY, szX, szY, x = 0, y = 0, irX, irY;
    CON_GetMetrics(&szX, &szY);
    while (1) {
        int Input = CheckInput(0);
        WPADData* data = WPAD_Data(0);
        irX = ((data->ir.x) * szX) / 560;
        irY = ((data->ir.y) * szY) / 420;

        if(!use_dpad) {
            x = irX;
            y = irY;
        }

        CON_GetPosition(&conX, &conY);
        char chr = numpad(x, y);

        if(!numpad_isIRinrange(irX, irY) && !use_dpad) {
            x = NUMPAD_X - 2;
            y = NUMPAD_Y - 2;
        }


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
                            if((buff[idx] * 10) + (chr - '0') <= 255) {
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

            case UP:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(y > (NUMPAD_Y - 2)) y -= 2;
                }
            break;

            case DOWN:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(y < (NUMPAD_Y - 2) + 2 * 3) y += 2;
                }
            break;

            case LEFT:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(x > NUMPAD_X + 4) x -= 4;
                }
            break;

            case RIGHT:
                if(!numpad_isIRinrange(irX, irY)) {
                    use_dpad = true;
                    if(x < NUMPAD_X + 4 * 2) x += 4;
                }                
            break;
        
            default:
            break;
        }
        
        if(numpad_isIRinrange(irX, irY)) {
            use_dpad = false;
        }
    }
}