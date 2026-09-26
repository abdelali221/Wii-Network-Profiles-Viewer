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