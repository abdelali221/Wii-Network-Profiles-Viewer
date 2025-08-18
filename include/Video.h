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