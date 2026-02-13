#ifndef _VIRTUALKB_H_
#define _VIRTUALKB_H_

typedef struct
{
    uint8_t ROW;
    uint8_t COL;
    char low_chr;
    char high_chr;
} virtualsymbol;

void ClearKeyboard();
char numpad(int irX, int irY);
char keyboard(bool shift, int irX, int irY);

#endif