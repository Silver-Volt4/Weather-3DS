#include "../util/graphics.h"
#include "app.h"

constexpr uint32_t CLEAR = C2D_Color32(0, 0, 0, 255);

App::App() : views(this)
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
        views.forecast.poll();
        break;
    case views.SETTINGS:
        views.settings.poll();
        break;
    default:
        break;
    }
}

void App::render()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    if (views.queuedUp == Views::NIL && views.fade != 0)
    {
        C2D_Fade(C2D_Color32(0, 0, 0, views.fade -= 15));
    }
    else if (views.queuedUp != Views::NIL)
    {
        if (views.fade != 255)
        {
            C2D_Fade(C2D_Color32(0, 0, 0, views.fade += 15));
        }
        else
        {
            views.current = views.queuedUp;
            views.queuedUp = Views::NIL;
        }
    }

    C2D_TargetClear(screen.top, CLEAR);
    C2D_SceneBegin(screen.top);

    switch (views.current)
    {
    case views.FORECAST:
        views.forecast.renderTop();
        break;
    case views.SETTINGS:
        views.settings.renderTop();
        break;
    default:
        break;
    }

    C2D_TargetClear(screen.bottom, CLEAR);
    C2D_SceneBegin(screen.bottom);

    switch (views.current)
    {
    case views.FORECAST:
        views.forecast.renderBottom();
        break;
    case views.SETTINGS:
        views.settings.renderBottom();
        break;
    default:
        break;
    }

    C3D_FrameEnd(0);
}

void App::changeView(App::Views::Types type)
{
    views.queuedUp = type;
    views.fade = 0;
}