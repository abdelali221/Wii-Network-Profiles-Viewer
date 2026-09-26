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

#ifndef _VIDEO_H_
#define _VIDEO_H_
#include <stdint.h>

#define WHITE_BG_BLACK_FG "\x1b[47;1m\x1b[30m"
#define RED_BG_WHITE_FG "\x1b[101;93m"
#define DEFAULT_BG_FG "\x1b[40m\x1b[37m"

void VideoInit();
void POSCursor(uint8_t X, uint8_t Y);
void ClearScreen();

#endif