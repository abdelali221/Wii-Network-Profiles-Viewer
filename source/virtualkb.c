#include "WiiLibs.h"
#include "virtualkb.h"
#include "Video.h"

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
    {0, 1, 130, '2'},
    {0, 2, '"', '3'},
    {0, 3, 39, '4'},
    {0, 4, '(', '5'},
    {0, 5, '-', '6'},
    {0, 6, 138, '7'},
    {0, 7, '_', '8'},
    {0, 8, 135,'9'},
    {0, 9, 133, '0'},
    {0, 10, ')', 248},
    {0, 11, '=', '+'},
    {1, 11, 156, '$'},
    {2, 10, '%', 151},
    {2, 11,  '*', '*'},
    {3, 10, '!', 21},
    {3, 9, ':', '/'},
    {3, 8, ';', '.'},
    {3, 7, ',', '?'},
    {3, 0, '<', '>'},
    {1, 10, 94, 94},
};

void ClearKeyboard() {
    for (size_t i = 0; i < 12; i++)
    {
        POSCursor(0, 15 + i);
        printf("\x1b[2K");
    }
}

char keyboard(bool shift, int irX, int irY) {
    char currChar = '\0';
    POSCursor(KEYBOARD_X, 15);
    printf(" _______________________________________________");
    POSCursor(KEYBOARD_X, 16);
    printf("|   |   |   |   |   |   |   |   |   |   |   |   |");
    POSCursor(KEYBOARD_X, 17);
    printf("|___|___|___|___|___|___|___|___|___|___|___|___|");
    POSCursor(KEYBOARD_X, 18);
    printf("|   |   |   |   |   |   |   |   |   |   |   |   |");
    POSCursor(KEYBOARD_X, 19);
    printf("|___|___|___|___|___|___|___|___|___|___|___|___|");
    POSCursor(KEYBOARD_X, 20);
    printf("|   |   |   |   |   |   |   |   |   |   |   |   |");
    POSCursor(KEYBOARD_X, 21);
    printf("|___|___|___|___|___|___|___|___|___|___|___|___|");
    POSCursor(KEYBOARD_X, 22);
    printf("|   |   |   |   |   |   |   |   |   |   |   |");
    POSCursor(KEYBOARD_X, 23);
    printf("|___|___|___|___|___|___|___|___|___|___|___|");
    POSCursor(KEYBOARD_X, 24);
    if (irX > KEYBOARD_X && irX < KEYBOARD_X + 31 && irY > 21 && irY < 24) {
        printf("|             %sSpace%s             | Backspace |", WHITE_BG_BLACK_FG, DEFAULT_BG_FG);
        currChar = ' ';
    } else if ((irX > KEYBOARD_X + 31 && irX < KEYBOARD_X + 42 && irY > 21 && irY < 24)) {
        printf("|             Space             | %sBackspace%s |", WHITE_BG_BLACK_FG, DEFAULT_BG_FG);
        currChar = '\b';
    } else {
        printf("|             Space             | Backspace |");
    }
    POSCursor(KEYBOARD_X, 25);
    printf("|_______________________________|___________|");
    for (size_t i = 0; i < 47; i++) {
        POSCursor(KEYBOARD_X + 2 + kbarray[i].COL * 4, 16 + kbarray[i].ROW * 2);
        if ((irX >= (KEYBOARD_X + 2 + kbarray[i].COL * 4) - 1 && irX <= (KEYBOARD_X + 2 + kbarray[i].COL * 4) + 1) && (irY >= 13 + kbarray[i].ROW * 2) && (irY <= 14 + kbarray[i].ROW * 2)) {
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
    return currChar;
}