#include "fractalascii.h"

#include "screen.h"
#include "camera.h"
#include "stats.h"
#include "mandelbrot.h"
#include "config.h"

#include "ncurses.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
    COLOR_PAIR_TITLE = 1,
    COLOR_PAIR_CONTROL_HELP,
    COLOR_PAIR_STATS

} ColorPairs;

#define AVERAGE(a, b) (((a) + (b)) / 2)
#define MAP(x, in_min, in_max, out_min, out_max) \
    (((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

static void FractalasciiUpdateDeltas(Fractalascii *pFascii)
{
    pFascii->dx = CameraGetWidth(&pFascii->cam) /
                  ScreenGetWidth(&pFascii->cam);

    pFascii->dy = CameraGetHeight(&pFascii->cam) /
                  ScreenGetHeight(&pFascii->cam) /
                  ScreenGetFontAspectRatio();
}

// makes sure zoom and deltas are in synch if screen dimensions changes
static void FractalasciiAdjustToScreen(Fractalascii *pFascii)
{
    static int prevWidth = 0;
    static int prevHeight = 0;

    if (prevWidth == 0 && prevHeight == 0)
    {

        CameraSetWidth(&pFascii->cam, (double)ScreenGetWidth() / (double)prevWidth);
        CameraSetHeight(&pFascii->cam, (double)ScreenGetHeight() / (double)prevHeight);

        FractalasciiUpdateDeltas(pFascii);

        CameraSetCenterX(&pFascii->cam, CameraGetCenterX(&pFascii->cam) +
                                            (double)(ScreenGetWidth() - prevWidth) * pFascii->dx / 2.0);

        CameraSetCenterY(&pFascii->cam, CameraGetCenterY(&pFascii->cam) +
                                            (double)(ScreenGetHeight() - prevHeight) * pFascii->dy / 2.0);
    }
    prevWidth = ScreenGetWidth();
    prevHeight = ScreenGetHeight();
}

static int32_t FractalasciiMapIterationToAsciiPallete(Fractalascii *pFascii,
                                                      uint32_t k)
{
    return (uint32_t)MAP(k,
                         (int32_t)StatsGetMin(pFascii->iterationStats),
                         (int32_t)StatsGetMax(pFascii->iterationStats),
                         (int32_t)0,
                         (int32_t)pFascii->asciiPalleteSize);
}

static uint32_t FractalasciiNormalize(Fractalascii *pFascii, uint32_t k)
{
    if (k > 2 && k < MandelbrotGetMaxIterations() &&
        (uint32_t)StatsGetMin(pFascii->iterationStats) <
            (uint32_t)MandelbrotGetMaxIterations())
    {
        k = FractalasciiMapIterationToAsciiPallete(pFascii, k);
    }
    return k;
}

static void FractalasciiDrawCharByIntensity(Fractalascii *pFascii,
                                            uint16_t x, uint16_t y, uint32_t i)
{
    ScreenSetAsciiPixel(x, y,
                        i <= pFascii->asciiPalleteSize ? pFascii->asciiPallete[i] : ' ');
}

uint32_t FractalasciiDrawPixel(Fractalascii *pFascii,
                               uint16_t pxl_x, uint16_t pxl_y,
                               double x, double y)
{
    uint32_t k, kn;
    k = MandelbrotGet(x, y);
    kn = (uint32_t)FractalasciiNormalize(pFascii, k);
    FractalasciiDrawCharByIntensity(pFascii, pxl_x, pxl_y, kn);
    return k;
}

static void FractalasciiDraw(Fractalascii *pFascii)
{
    Stats iterationStats;
    StatsReset(&iterationStats);
    FractalasciiUpdateDeltas(pFascii);
    double x, y;
    int32_t pxl_x, pxl_y;
    for (pxl_y = 0; pxl_y < ScreenGetHeight(); pxl_y++)
    {
        y = CameraGetYmax(&pFascii->cam) - pxl_y * pFascii->dy;
        for (pxl_x = 0; pxl_x < ScreenGetWidth(); pxl_x++)
        {
            x = CameraGetXmin(&pFascii->cam) + pxl_x * pFascii->dx;
            uint32_t k = FractalasciiDrawPixel(pFascii, pxl_x, pxl_y, x, y);

            if (k > 2 && k < MandelbrotGetMaxIterations())
            {
                StatsCollect(k, &iterationStats);
            }
        }
    }
    StatsRecord(&iterationStats, &pFascii->iterationStats);
}

static void FractalasciiDrawStats(Fractalascii *pFascii)
{
    if (pFascii->showStats)
    {
        int y, x;
        y = ScreenGetHeight() - 1 - 3;
        x = 0;
        char statsStr[0x200];
        StatsToString(pFascii->iterationStats, "Mandelbrot iteration", statsStr);

        init_pair(COLOR_PAIR_STATS, COLOR_CYAN, 0);
        attron(COLOR_PAIR(COLOR_PAIR_STATS));
        mvprintw(y++, x, statsStr);
        mvprintw(y++, x, "center [x: %.20lf, y: %.20lf]",
                 CameraGetCenterX(&pFascii->cam), CameraGetCenterY(&pFascii->cam));
        mvprintw(y++, x, "camera [width: %e, height: %e]",
                 CameraGetWidth(&pFascii->cam), CameraGetHeight(&pFascii->cam));
        attroff(COLOR_PAIR(COLOR_PAIR_STATS));
    }
}

static void NcursesAttributesDemo(WINDOW *wnd)
{
    const int attributes[] =
        {
            A_NORMAL,     // Normal display (no highlight)
            A_STANDOUT,   // Best highlighting mode of the terminal.
            A_UNDERLINE,  // Underlining
            A_REVERSE,    // Reverse video
            A_BLINK,      // Blinking
            A_DIM,        // Half bright
            A_BOLD,       // Extra bright or bold
            A_PROTECT,    // Protected mode
            A_INVIS,      // Invisible or blank mode
            A_ALTCHARSET, // Alternate character set
            A_CHARTEXT,   // Bit-mask to extract a character
                          //COLOR_PAIR(n)   // Color-pair number n
        };

    move(0, 0);
    clear();

    short color = 0;
    int x = 0, y = 0;
    int att;
    for (att = 0; att < (sizeof(attributes) / sizeof(int)); att++)
    {
        //attron(attributes[att]);
        printw("%d: ", att);

        for (x = 0; x < 64; x++)
        {
            //attron(COLOR_PAIR(color));

            ScreenSetColor(color & 0x111, (color & 0b1110000) >> 4);

            addch('A' + x);
            color++;
            color %= 0x3F;

            //attroff(COLOR_PAIR(color));
        }
        y++;
        addch('\n');
        //attroff(attributes[att]);
    }
    while (1)
    {
        refresh();
    };
}

static void FractalasciiHelpWindow(Fractalascii *pFascii)
{
    const char *sTitle = "Controls";
    char *saControls[] =
        {
            "Navigation:",
            "  `w`, `a`, `s`, `d` - move camera",
            "  `e` - zoom in 10%",
            "  `q` - zoom out 10%",
            "  `r` - reset camera",
            "  `0-9` - interesting camera presets",
            "Common:",
            "  `h` - see this help window",
            "  `g` - see stats",
            "  `c` - exit",
            "",
            "NOTE. To see the real beauty, decrease",
            "terminal symbolic resolution using",
            "`CTRL` + `-`, increase - `CTRL` + `+`",
        };

    const uint32_t saControlsSize = (sizeof(saControls) / sizeof(char *));

    static WINDOW *ctrlWin;
    static int h, w;

    static bool showHelpInit = false;
    if (pFascii->showHelpWindow && !showHelpInit)
    {
        h = 2 /*title and hline */ + saControlsSize + 2 /* horz borders */;
        w = 2;
        int i, slen;
        for (i = 0; i < saControlsSize; i++)
        {
            slen = strlen(saControls[i]);
            if (slen + 2 > w)
            {
                w = slen + 2;
            }
        }

        ctrlWin = derwin(stdscr, h, w,
                         ScreenGetHeight() / 2 - h / 2,
                         ScreenGetWidth() / 2 - w / 2);

        showHelpInit = true;
    }

    if (!pFascii->showHelpWindow)
    {
        showHelpInit = false;
    }

    // draw control help window
    if (pFascii->showHelpWindow)
    {
        wclear(ctrlWin);
        int y = 0, x = 0;
        box(ctrlWin, 0, 0);
        y++;
        x++;
        mvwprintw(ctrlWin, y++, w / 2 - strlen(sTitle) / 2, sTitle);
        mvwhline(ctrlWin, y++, x, '-', w - 2);

        init_pair(COLOR_PAIR_CONTROL_HELP, COLOR_YELLOW, 0);
        wattron(ctrlWin, COLOR_PAIR(COLOR_PAIR_CONTROL_HELP));
        int i;
        for (i = 0; i < saControlsSize; i++)
        {
            mvwprintw(ctrlWin, y++, x, saControls[i]);
        }
        wattroff(ctrlWin, COLOR_PAIR(COLOR_PAIR_CONTROL_HELP));

        NcursesAttributesDemo(ctrlWin);

        wrefresh(ctrlWin);
    }
}

static void FractalasciiReactToControls(Fractalascii *pFascii)
{
    const double s = 0.1; // step in %
    double horzStepSize = 20.00; // move scale
    double stepHorz = (double)CameraGetWidth(&pFascii->cam) / horzStepSize;
    double stepVert = (double)CameraGetHeight(&pFascii->cam) / horzStepSize /
                      ScreenGetFontAspectRatio();

    int c = getch();

    if (c == 'h') // toggle controls help window
    {
        pFascii->showHelpWindow = !pFascii->showHelpWindow;
    }
    if (c == 'g') // show stats
    {
        pFascii->showStats = !pFascii->showStats;
    }
    if (c == 'e') // zoom in 10 %
    {
        CameraZoom(&pFascii->cam, 1.00 - s);
        pFascii->showHelpWindow = false;
    }
    if (c == 'E') // zoom in 50 %
    {
        CameraZoom(&pFascii->cam, 1.00 - s * 5);
        pFascii->showHelpWindow = false;
    }
    if (c == 'q') // zoom out 10 %
    {
        CameraZoom(&pFascii->cam, 1.00 + s);
        pFascii->showHelpWindow = false;
    }
    if (c == 'Q') // zoom out 50 %
    {
        CameraZoom(&pFascii->cam, 1.00 + s * 5);
        pFascii->showHelpWindow = false;
    }
    if (c == 'w') // move camera up
    {
        CameraSetCenterY(&pFascii->cam,
                         CameraGetCenterY(&pFascii->cam) + stepVert);
        pFascii->showHelpWindow = false;
    }
    if (c == 'a') // move camera left
    {
        CameraSetCenterX(&pFascii->cam,
                         CameraGetCenterX(&pFascii->cam) - stepHorz);
        pFascii->showHelpWindow = false;
    }
    if (c == 's') // move camera down
    {
        CameraSetCenterY(&pFascii->cam,
                         CameraGetCenterY(&pFascii->cam) - stepVert);
        pFascii->showHelpWindow = false;
    }
    if (c == 'd') // move camera right
    {
        CameraSetCenterX(&pFascii->cam,
                         CameraGetCenterX(&pFascii->cam) + stepHorz);
        pFascii->showHelpWindow = false;
    }
    if (c == 'r') // reset camera
    {
        CameraSetPosition(&pFascii->cam, -2.00, 0.00, 6.00);
        pFascii->showHelpWindow = false;
    }

    if (c == 'c') // exit program
    {
        // TODO fractalascii deconstruction, memory freeing
        ScreenExit();
    }

    const struct
    {
        double x;
        double y;
        double width;
    } aPresets[] =
        {
            {-0.722, 0.246, 0.019},
            {-0.7463, 0.1102, 0.005},
            {-0.7453, 0.1127, 6.5E-4},
            {-0.74529, 0.113075, 1.5E-4},
            {-0.745428, 0.113009, 3.0E-5},
            {-0.16, 1.0405, 0.026},
            {-0.925, 0.266, 0.032},
            {-1.25066, 0.02012, 1.7E-4},
            {-0.748, 0.1, 0.0014},
            {-0.23512922655760265522, y : 0.82720650024941300149, 4.0E-5}

        };

    if (c >= '0' && c <= '9')
    {
        int i = c - '0';
        CameraSetPosition(&pFascii->cam, aPresets[i].x, aPresets[i].y,
                          aPresets[i].width);
        pFascii->showHelpWindow = false;
    }
}

static void FractalasciiRun(void *p)
{
    Fractalascii *pFascii = (Fractalascii *)p;
    FractalasciiAdjustToScreen(pFascii);
    FractalasciiDraw(pFascii);
    FractalasciiDrawStats(pFascii);
    FractalasciiHelpWindow(pFascii);

    ScreenRefresh();

    FractalasciiReactToControls(pFascii);
}

static void FractalasciiReset(Fractalascii *pFascii)
{
    ScreenReset();
    CameraReset(&pFascii->cam);
    StatsReset(&pFascii->iterationStats);
    pFascii->showStats = CONFIG_SHOW_STATS_BY_DEFAULT;
    pFascii->showHelpWindow = CONFIG_SHOW_HELP_WINDOW_BY_DEFAULT;
}

static void FractalasciiInit(Fractalascii *pFascii,
                             const char *asciiPallete, double fontAspectRatio)
{
    // ncurse color pairs
    // init_pair(COLOR_PAIR_CONTROL_HELP, COLOR_YELLOW, 0);
    // init_pair(COLOR_PAIR_STATS, COLOR_BLUE, 0);

    pFascii->asciiPallete = (char *)asciiPallete;
    pFascii->asciiPalleteSize = strlen(asciiPallete);

    ScreenSetFontAspectRatio(fontAspectRatio);
    CameraSetPosition(&pFascii->cam, -1.00, 0.00, 6.0); //default

    uint8_t init = 50;
    while (init--)
    {
        FractalasciiAdjustToScreen(pFascii);
        FractalasciiDraw(pFascii);
    }
}

void FractalasciiStart(const char *asciiPallete, double fontAspectRatio)
{
    Fractalascii fa;
    FractalasciiReset(&fa);
    FractalasciiInit(&fa, asciiPallete, fontAspectRatio);
    ScreenRun(FractalasciiRun, (void *)&fa);
}
