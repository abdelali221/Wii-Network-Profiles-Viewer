#include "Input.h"
#include "WiiLibs.h"
#include "Video.h"

int CheckWPAD(int pad) {
    WPAD_ScanPads();

	u32 pressed = WPAD_ButtonsDown(pad);
    
    if (pressed & WPAD_BUTTON_UP) {
        
        return UP;

    } else if (pressed & WPAD_BUTTON_DOWN) {

        return DOWN;

    } else if (pressed & WPAD_BUTTON_LEFT) {

        return LEFT;

    } else if (pressed & WPAD_BUTTON_RIGHT) {

        return RIGHT;

    } else if (pressed & WPAD_BUTTON_B) {
    
        return b_B;
    
    } else if (pressed & WPAD_BUTTON_A) {
    
        return b_A;
    
    } else if (pressed & WPAD_BUTTON_1) {
    
        return ONE;
    
    } else if (pressed & WPAD_BUTTON_2) {
    
        return TWO;
    
    } else if (pressed & WPAD_BUTTON_MINUS) {
    
        return MINUS;
    
    } else if (pressed & WPAD_BUTTON_PLUS) {
    
        return PLUS;
    
    } else if (pressed & WPAD_BUTTON_HOME) {
    
        return HOME;
    
    }
    return 0;
}

void InputInit() {
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
}