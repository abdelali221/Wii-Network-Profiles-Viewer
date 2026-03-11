#include "Input.h"
#include "WiiLibs.h"
#include "Video.h"

int CheckInput(int pad) {
    VIDEO_WaitVSync();
    VIDEO_WaitVSync();
    
    WPAD_ScanPads();
    PAD_ScanPads();

	u32 pressed = WPAD_ButtonsDown(pad);
    u32 gcpressed = PAD_ButtonsDown(pad);
    
    if (pressed & WPAD_BUTTON_UP || gcpressed & PAD_BUTTON_UP) {
        
        return UP;

    } else if (pressed & WPAD_BUTTON_DOWN || gcpressed & PAD_BUTTON_DOWN) {

        return DOWN;

    } else if (pressed & WPAD_BUTTON_LEFT || gcpressed & PAD_BUTTON_LEFT) {

        return LEFT;

    } else if (pressed & WPAD_BUTTON_RIGHT || gcpressed & PAD_BUTTON_RIGHT) {

        return RIGHT;

    } else if (pressed & WPAD_BUTTON_B || gcpressed & PAD_BUTTON_B) {
    
        return b_B;
    
    } else if (pressed & WPAD_BUTTON_A || gcpressed & PAD_BUTTON_A) {
    
        return b_A;
    
    } else if (pressed & WPAD_BUTTON_1 || gcpressed & PAD_BUTTON_X) {
    
        return ONE;
    
    } else if (pressed & WPAD_BUTTON_2 || gcpressed & PAD_BUTTON_Y) {
    
        return TWO;
    
    } else if (pressed & WPAD_BUTTON_MINUS || gcpressed & PAD_TRIGGER_R) {
    
        return MINUS;
    
    } else if (pressed & WPAD_BUTTON_PLUS || gcpressed & PAD_TRIGGER_L) {
    
        return PLUS;
    
    } else if (pressed & WPAD_BUTTON_HOME || gcpressed & PAD_BUTTON_START) {
    
        return HOME;
    
    }
    return 0;
}

void InputInit() {
    PAD_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
}