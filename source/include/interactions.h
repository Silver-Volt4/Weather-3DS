#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <math.h>

struct Screen
{
    static constexpr int SCREEN_HEIGHT = 240;
    static constexpr int TOP_SCREEN_WIDTH = 400;
    static constexpr int BOTTOM_SCREEN_WIDTH = 320;

    C3D_RenderTarget *top;
    C3D_RenderTarget *bottom;

    void init();
};

struct touchPositionDiff
{
    short int dx;
    short int dy;
    float distance() const;
};

class InputHandler
{
public:
    uint32_t kDown;
    uint32_t kHeld;
    uint32_t kUp;
    touchPosition touchStart;
    touchPosition touchCurrent;
    touchPosition touchLast;
    touchPositionDiff touchDelta;
    void scan();
    touchPositionDiff drag() const;
};