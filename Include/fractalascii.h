#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "screen.h"
#include "camera.h"
#include "stats.h"

typedef struct Fractalascii_
{
    Camera cam;
    double dx, dy; // camera plain per screen pixel
    char *asciiPallete;
    uint16_t asciiPalleteSize;
    Stats iterationStats;
    bool showStats;
    bool showHelpWindow;

} Fractalascii;

void FractalasciiStart(const char *asciiPallete, double pixelAspectRatio);

