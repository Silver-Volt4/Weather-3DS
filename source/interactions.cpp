#include <3ds.h>
#include "include/interactions.h"

void Screen::init()
{
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
}

void InputHandler::scan()
{
    hidScanInput();

    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    kUp = hidKeysUp();

    touchLast = touchCurrent;
    hidTouchRead(&touchCurrent);

    if (kDown & KEY_TOUCH)
    {
        touchStart = touchCurrent;
    }
    else if (kHeld & KEY_TOUCH)
    {
        touchDelta.dx = touchCurrent.px - touchLast.px;
        touchDelta.dy = touchCurrent.py - touchLast.py;
    }
    else if (kUp & KEY_TOUCH)
    {
        touchDelta.dx = 0;
        touchDelta.dy = 0;
    }
}

touchPositionDiff InputHandler::drag() const
{
    touchPositionDiff d;
    d.dx = touchCurrent.px - touchStart.px;
    d.dy = touchCurrent.py - touchStart.py;
    return d;
}

float touchPositionDiff::distance() const
{
    return hypotf(dx, dx);
}