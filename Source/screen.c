#include "screen.h"

#include "config.h"

#include <ncurses.h>
#include <stdio.h>
#include "timer.h"

#define DISP_MAX_FPS CONFIG_MAX_FPS

Screen screen;

void ScreenReset()
{
    screen.fontAspectRatio = 1.00;
    screen.showFPS = DISP_MAX_FPS;
    screen.exitRequest = 0;
}

int ScreenGetWidth()
{
    return getmaxx(stdscr) + 1;
}

int ScreenGetHeight()
{
    return getmaxy(stdscr) + 1;
}

double ScreenGetRatio()
{
    return (double)ScreenGetWidth() / (double)ScreenGetHeight();
}

// note. terminal ascii symbols usually have 2.0 aspect ratio
void ScreenSetFontAspectRatio(double fontAspectRatio)
{
    screen.fontAspectRatio = fontAspectRatio;
}

double ScreenGetFontAspectRatio()
{
    return screen.fontAspectRatio;
}

short curs_color(int fg)
{
    switch (7 & fg)
    {       /* RGB */
    case 0: /* 000 */
        return (COLOR_BLACK);
    case 1: /* 001 */
        return (COLOR_BLUE);
    case 2: /* 010 */
        return (COLOR_GREEN);
    case 3: /* 011 */
        return (COLOR_CYAN);
    case 4: /* 100 */
        return (COLOR_RED);
    case 5: /* 101 */
        return (COLOR_MAGENTA);
    case 6: /* 110 */
        return (COLOR_YELLOW);
    case 7: /* 111 */
        return (COLOR_WHITE);
    default:
        return (COLOR_WHITE);
    }
}

int colornum(int fg, int bg)
{
    int B, bbb, ffff;

    B = 1 << 7;
    bbb = (7 & bg) << 4;
    ffff = 7 & fg;

    return (B | bbb | ffff);
}

static void init_colorpairs(void)
{
    int fg, bg;
    int colorpair;

    for (bg = 0; bg <= 7; bg++)
    {
        for (fg = 0; fg <= 7; fg++)
        {
            colorpair = colornum(fg, bg);
            init_pair(colorpair, curs_color(fg), curs_color(bg));
        }
    }
}

int ScreenColorNum(int fg, int bg)
{
    int B, bbb, ffff;

    B = 1 << 7;
    bbb = (7 & bg) << 4;
    ffff = 7 & fg;

    return (B | bbb | ffff);
}

static int is_bold(int fg)
{
    /* return the intensity bit */

    int i;

    i = 1 << 3;
    return (i & fg);
}

void ScreenSetColor(int fg, int bg)
{
    /* set the color pair (colornum) and bold/bright (A_BOLD) */

    attron(COLOR_PAIR(ScreenColorNum(fg, bg)));
    if (is_bold(fg))
    {
        attron(A_BOLD);
    }
}

void ScreenResetColor(int fg, int bg)
{
    /* unset the color pair (colornum) and
       bold/bright (A_BOLD) */

    attroff(COLOR_PAIR(ScreenColorNum(fg, bg)));
    if (is_bold(fg))
    {
        attroff(A_BOLD);
    }
}

static void ScreenStart()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_colorpairs();
    clear();
    refresh();
}

static void ScreenShowFrame()
{
    static Timer fpstimer;
    uint32_t fps;
    timerGetFPS(&fpstimer, &fps);
    mvprintw(0, 0, "[%ldFPS]", fps);
}

static void ScreenProcessFPSOverlay()
{
    if (screen.showFPS)
    {
        ScreenShowFrame();
    }
}

void ScreenSetAsciiPixel(int x, int y, char asciiPixel)
{
    mvaddch(y, x, asciiPixel);
}

static void ScreenLimitFPS()
{
    const size_t maxFps = DISP_MAX_FPS;
    if (maxFps)
    {
        timerDelayMs(1000 / maxFps); //todo legit, non-blocking
    }
}

void ScreenExit()
{
    screen.exitRequest = 1;
}

static void ScreenEnd()
{
    endwin();
}

void ScreenRefresh()
{
    ScreenProcessFPSOverlay();
}

void ScreenRun(void (*drawFnc)(void *p), void *p)
{
    ScreenStart();

    while (!screen.exitRequest)
    {
        drawFnc(p);
        ScreenLimitFPS();
    }
    ScreenEnd();
}
