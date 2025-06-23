#include "../util/graphics.h"
#include "app.h"

constexpr uint32_t CLEAR = C2D_Color32(0, 0, 0, 255);
constexpr uint32_t WHITE = C2D_Color32(255, 255, 255, 255);
constexpr uint32_t SHADOW = C2D_Color32(0, 0, 0, 100);
constexpr uint32_t BLUE_GRADIENT_TOP = C2D_Color32(2, 167, 225, 255);
constexpr uint32_t BLUE_GRADIENT_BOTTOM = C2D_Color32(7, 51, 149, 255);

App::App()
{
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    httpcInit(0);
    screen.init();
    assets.init();

    // consoleInit(GFX_BOTTOM, NULL);
}

App::~App()
{
    assets.deinit();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

bool App::frame()
{
    beforeRender();
    render();
    return running;
}

void App::beforeRender()
{
    input.scan();
    httpWorker.poll();
    if (input.kDown & KEY_START)
    {
        running = false;
        return;
    }

    switch (views.current)
    {
    case views.FORECAST:
        views.forecast.poll(*this);
        break;
    }
}

void App::render()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    static uint8_t fade = 255;

    if (fade != 0)
    {
        C2D_Fade(C2D_Color32(0, 0, 0, fade -= 5));
    }

    C2D_TargetClear(screen.top, CLEAR);
    C2D_SceneBegin(screen.top);

    switch (views.current)
    {
    case views.FORECAST:
        views.forecast.renderTop(*this);
        break;
    }

    C2D_TargetClear(screen.bottom, CLEAR);
    C2D_SceneBegin(screen.bottom);

    switch (views.current)
    {
    case views.FORECAST:
        views.forecast.renderBottom(*this);
        break;
    }

    C3D_FrameEnd(0);
}