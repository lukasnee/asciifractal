#pragma once

#include <stdint.h>

typedef struct Screen_
{
    double fontAspectRatio;
    uint8_t showFPS;
    uint8_t exitRequest;
} Screen;

void ScreenReset();
int ScreenGetWidth();
int ScreenGetHeight();
double ScreenGetRatio();
void ScreenSetFontAspectRatio(double fontAspectRatio);
double ScreenGetFontAspectRatio();

void ScreenSetColor(int fg, int bg);
void ScreenResetColor(int fg, int bg);

void ScreenSetAsciiPixel(int x, int y, char asciiPixel);
void ScreenRefresh();
void ScreenRun(void (*drawFnc)(void *p), void *p);
void ScreenExit();
