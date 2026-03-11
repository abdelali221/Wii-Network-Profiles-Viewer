#ifndef _VIRTUALKB_H_
#define _VIRTUALKB_H_

typedef struct
{
    uint8_t ROW;
    uint8_t COL;
    char low_chr;
    char high_chr;
} virtualsymbol;

extern u8 KEYBOARD_X;
extern u8 KEYBOARD_Y;
extern u8 NUMPAD_X;
extern u8 NUMPAD_Y;

void ClearKeyboard();
char numpad(int irX, int irY);
bool numpad_isIRinrange(int irX, int irY);
char keyboard(bool shift, int irX, int irY);
bool keyboard_isIRinrange(int irX, int irY);

#endif