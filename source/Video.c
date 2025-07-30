#include "Video.h"
#include "WiiLibs.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void VideoInit() {
	VIDEO_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	console_init(xfb,20,20,rmode->fbWidth-20,rmode->xfbHeight-20,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);

	VIDEO_SetNextFramebuffer(xfb);

	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

	VIDEO_SetBlack(false);

	VIDEO_Flush();

	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	POSCursor(0, 0);
}

void POSCursor(uint8_t X, uint8_t Y) {
	printf("\x1b[%d;%dH", Y, X);
}

void ClearScreen() {
	printf("\x1b[2J");
}