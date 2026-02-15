#include "WiiLibs.h"
#include "virtualkb.h"
#include "Video.h"

#define RIGHT_MIDDLE_LINK 185
#define VER_BAR 186
#define RIGHT_TOP_CORNER 187
#define RIGHT_BOTTOM_CORNER 188
#define LEFT_BOTTOM_CORNER 200
#define LEFT_TOP_CORNER 201
#define BOTTOM_MIDDLE_LINK 202
#define TOP_MIDDLE_LINK 203
#define LEFT_MIDDLE_LINK 204
#define HOR_BAR 205
#define CENTER_PLUS_LINK 206

u8 KEYBOARD_X = 13;
u8 KEYBOARD_Y = 14;
u8 NUMPAD_X = 26;
u8 NUMPAD_Y = 14;

virtualsymbol kbarray[48] = {
    {1, 0, 'a', 'A'},
    {3, 5, 'b', 'B'},
    {3, 3, 'c', 'C'},
    {2, 2, 'd', 'D'},
    {1, 2, 'e', 'E'},
    {2, 3, 'f', 'F'},
    {2, 4, 'g', 'G'},
    {2, 5, 'h', 'H'},
    {1, 7, 'i', 'I'},
    {2, 6, 'j', 'J'},
    {2, 7, 'k', 'K'},
    {2, 8, 'l', 'L'},
    {2, 9, 'm', 'M'},
    {3, 6, 'n', 'N'},
    {1, 8, 'o', 'O'},
    {1, 9, 'p', 'P'},
    {2, 0, 'q', 'Q'},
    {1, 3, 'r', 'R'},
    {2, 1, 's', 'S'},
    {1, 4, 't', 'T'},
    {1, 6, 'u', 'U'},
    {3, 4, 'v', 'V'},
    {3, 1, 'w', 'W'},
    {3, 2, 'x', 'X'},
    {1, 5, 'y', 'Y'},
    {1, 1, 'z', 'Z'},
    {0, 0, '&', '1'},
    {0, 1, '7', '2'},
    {0, 2, '"', '3'},
    {0, 3, '4', '4'},
    {0, 4, '(', '5'},
    {0, 5, '-', '6'},
    {0, 6, '7', '7'},
    {0, 7, '_', '8'},
    {0, 8, '9','9'},
    {0, 9, '0', '0'},
    {0, 10, ')', 248},
    {0, 11, '=', '+'},
    {1, 11, '$', '$'},
    {2, 10, '%', '%'},
    {2, 11,  '*', '*'},
    {3, 10, '!', '!'},
    {3, 9, ':', '/'},
    {3, 8, ';', '.'},
    {3, 7, ',', '?'},
    {3, 0, '<', '>'},
    {1, 10, 94, 94},
};

virtualsymbol numpadarray[] = {
    {2, 0, '1','1'},
    {2, 1, '2','2'},
    {2, 2, '3','3'},
    {1, 0, '4','4'},
    {1, 1, '5','5'},
    {1, 2, '6','6'},
    {0, 0, '7','7'},
    {0, 1, '8','8'},
    {0, 2, '9','9'},
    {3, 1, '0','0'},
    {3, 2, '<','<'},
    {3, 0, '.','.'},
};

void SetKBXandY(u8 x, u8 y) {
    KEYBOARD_X = x;
    KEYBOARD_Y = y;
}

void ClearKeyboard() {
    for (size_t i = 0; i < 13; i++)
    {
        POSCursor(0, KEYBOARD_Y + i);
        printf("\x1b[2K");
    }
}

char numpad(int irX, int irY) {
    char currChar = '\0';
    for(int i = 0; i < 5; i++) {
        POSCursor(NUMPAD_X, NUMPAD_Y + i * 2);
        if(i == 0) {
            putchar(LEFT_TOP_CORNER);
            for(int j = 0; j < 3; j++) {
                putchar(HOR_BAR);
                putchar(HOR_BAR);
                putchar(HOR_BAR);
                if(j == 2) break;
                putchar(TOP_MIDDLE_LINK);
            }
            putchar(RIGHT_TOP_CORNER);
        } else if(i == 4) {
            putchar(LEFT_BOTTOM_CORNER);
            for(int j = 0; j < 3; j++) {
                putchar(HOR_BAR);
                putchar(HOR_BAR);
                putchar(HOR_BAR);
                if(j == 2) break;
                putchar(BOTTOM_MIDDLE_LINK);
            }
            putchar(RIGHT_BOTTOM_CORNER);
        } else {
            putchar(LEFT_MIDDLE_LINK);
            for(int j = 0; j < 3; j++) {
                putchar(HOR_BAR);
                putchar(HOR_BAR);
                putchar(HOR_BAR);
                if(j == 2) break;
                putchar(CENTER_PLUS_LINK);
            }
            putchar(RIGHT_MIDDLE_LINK);
        }
        if(i == 4) break;
        for(int j = 0; j < 4; j++) {
            POSCursor(NUMPAD_X + j * 4, NUMPAD_Y + 1 + i * 2);
            putchar(VER_BAR);
        }
    }

    for (size_t i = 0; i < 12; i++) {
        POSCursor(NUMPAD_X + 2 + numpadarray[i].COL * 4, NUMPAD_Y + 1 + numpadarray[i].ROW * 2);
        if ((irX > (NUMPAD_X + 2 + numpadarray[i].COL * 4) - 3 && irX < (NUMPAD_X + 2 + numpadarray[i].COL * 4) + 2) && (irY >= NUMPAD_Y - 2 + numpadarray[i].ROW * 2) && (irY <= NUMPAD_Y - 1 + numpadarray[i].ROW * 2)) {
            printf("%s", WHITE_BG_BLACK_FG);
            currChar = numpadarray[i].low_chr;
        }
        putchar(numpadarray[i].low_chr);
        printf("%s", DEFAULT_BG_FG);
    }
    return currChar;
}

char keyboard(bool shift, int irX, int irY) {
    char currChar = '\0';

    POSCursor(KEYBOARD_X, KEYBOARD_Y);
    putchar(LEFT_TOP_CORNER);
    for(int i = 1; i < 48; i++) {
        if(i % 4 == 0) {
            putchar(TOP_MIDDLE_LINK);
        } else {
            putchar(HOR_BAR);
        }
    }
    putchar(RIGHT_TOP_CORNER);
    for(int i = 0; i < 3; i++) {
        POSCursor(KEYBOARD_X, KEYBOARD_Y + 1 + i * 2);
        putchar(VER_BAR);
        for(int j = 0; j < 13; j++) {
            POSCursor(KEYBOARD_X + j * 4, KEYBOARD_Y + 1 + i * 2);
            putchar(VER_BAR);
        }
        POSCursor(KEYBOARD_X, KEYBOARD_Y + 2 + i * 2);
        putchar(LEFT_MIDDLE_LINK);
        for(int j = 0; j < 12; j++) {
            putchar(HOR_BAR);
            putchar(HOR_BAR);
            putchar(HOR_BAR);
            if(j == 11) break;
            putchar(CENTER_PLUS_LINK);
        }
        if(i < 2) {
            putchar(RIGHT_MIDDLE_LINK);
        } else {
            putchar(189);
        }
    }
    POSCursor(KEYBOARD_X, KEYBOARD_Y + 7);
    putchar(VER_BAR);
    for(int j = 0; j < 12; j++) {
        POSCursor(KEYBOARD_X + j * 4, KEYBOARD_Y + 7);
        putchar(VER_BAR);
    }
    POSCursor(KEYBOARD_X, KEYBOARD_Y + 8);
    putchar(LEFT_MIDDLE_LINK);
    for(int j = 0; j < 11; j++) {
        putchar(HOR_BAR);
        putchar(HOR_BAR);
        putchar(HOR_BAR);
        if(j == 10) break;
        if(j == 7) {
            putchar(CENTER_PLUS_LINK);
        } else {
            putchar(202);
        }
    }
    putchar(RIGHT_MIDDLE_LINK);
    POSCursor(KEYBOARD_X, KEYBOARD_Y + 9);
    if (irX > KEYBOARD_X && irX < KEYBOARD_X + 31 && irY > 19 && irY < 23) {
        printf("%c             %sSpace%s             %c Backspace %c", VER_BAR, WHITE_BG_BLACK_FG, DEFAULT_BG_FG, VER_BAR, VER_BAR);
        currChar = ' ';
    } else if ((irX > KEYBOARD_X + 31 && irX < KEYBOARD_X + 42 && irY > 19 && irY < 23)) {
        printf("%c             Space             %c %sBackspace%s %c", VER_BAR, VER_BAR, WHITE_BG_BLACK_FG, DEFAULT_BG_FG, VER_BAR);
        currChar = '\b';
    } else {
        printf("%c             Space             %c Backspace %c", VER_BAR, VER_BAR, VER_BAR);
    }
    POSCursor(KEYBOARD_X, KEYBOARD_Y + 10);
    putchar(LEFT_BOTTOM_CORNER);
    for(int i = 0; i < 11; i++) {
        putchar(HOR_BAR);
        putchar(HOR_BAR);
        putchar(HOR_BAR);
        if(i == 7) {
            putchar(202);
        } else if(i == 10) {
            putchar(RIGHT_BOTTOM_CORNER);
        } else {
            putchar(HOR_BAR);
        }
    }
    for (size_t i = 0; i < 47; i++) {
        POSCursor(KEYBOARD_X + 2 + kbarray[i].COL * 4, KEYBOARD_Y + 1 + kbarray[i].ROW * 2);
        if ((irX > (KEYBOARD_X + 2 + kbarray[i].COL * 4) - 3 && irX < (KEYBOARD_X + 2 + kbarray[i].COL * 4) + 2) && (irY >= KEYBOARD_Y - 2 + kbarray[i].ROW * 2) && (irY <= KEYBOARD_Y - 1 + kbarray[i].ROW * 2)) {
            printf("%s", WHITE_BG_BLACK_FG);
            if (shift) {
                currChar = kbarray[i].high_chr;
            } else {
                currChar = kbarray[i].low_chr;
            }
        }
        if(shift) {
            putchar(kbarray[i].high_chr);
        } else {
            putchar(kbarray[i].low_chr);
        }
        printf("%s", DEFAULT_BG_FG);
    }
    POSCursor(KEYBOARD_X, KEYBOARD_Y + 11);
    printf("B : Shift");
    if (currChar == '\0') printf(" / Please aim at the keyboard with your WiiMote");
    else printf("%*c", 48, ' ');
    return currChar;
}