#include "include/app.h"
#include "include/graphics.h"

App::App()
{
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    screen.init();
    assets.init();
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
    if (input.kDown & KEY_START)
    {
        running = false;
        return;
    }
}

void App::render()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(screen.top, clearColor);
    C2D_SceneBegin(screen.top);
    renderTop();

    C2D_TargetClear(screen.bottom, clearColor);
    C2D_SceneBegin(screen.bottom);
    renderBottom();

    C3D_FrameEnd(0);
}

template <uint8_t S>
void parseTemperature(Assets &assets, SpriteTextRenderer<S> &str, int8_t tempValue, bool celsius)
{
    if (tempValue < 0)
    {
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 11)); // minus
        tempValue = std::abs(tempValue);
    }

    bool leading = true;
    for (uint8_t order = 100; order > 0; order /= 10)
    {
        uint8_t digit = tempValue / order;
        if (digit || !leading)
        {
            leading = false;
            str.addChar(C2D_SpriteSheetGetImage(assets.numbers, digit));
        }
        tempValue -= digit * order;
    }

    if (celsius)
    {
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 10)); // °
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 12)); // C
    }
    else
    {
        str.addSpace();
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 13)); // index of F
    }
}

class GlanceView
{
    Assets *assets;
    struct : TextRenderer<128>
    {
        C2D_Text cityName;

        void setCityName(std::string name)
        {
            clearBuffer();
            configureText(&cityName, name.c_str());
        }
    } texts;

    SpriteTextRenderer<8> str;

public:
    GlanceView(Assets *assets, std::string name, int8_t tempValue, uint8_t weatherState, bool celsius) : assets(assets)
    {
        texts.setCityName(name);

        str.addChar(C2D_SpriteSheetGetImage(assets->icons, weatherState), 2.0);

        parseTemperature(*assets, str, tempValue, celsius);
    }

    void draw(float scale)
    {
        static C2D_ImageTint tint;

        constexpr float GLANCE_TEXT_X = Screen::TOP_SCREEN_WIDTH / 2;
        constexpr float GLANCE_TEXT_Y = Screen::SCREEN_HEIGHT / 2;

        C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 0, 100), 1);
        str.render(GLANCE_TEXT_X + (10 * scale), GLANCE_TEXT_Y + (10 * scale), scale, &tint);

        C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 255), 0);
        str.render(GLANCE_TEXT_X, GLANCE_TEXT_Y, scale, &tint);

        C2D_DrawImageAt(C2D_SpriteSheetGetImage(assets->gui, 0), 40, 0, 0);
        C2D_DrawText(&texts.cityName, C2D_AlignCenter | C2D_WithColor, 200, 6, 0, 0.8, 0.8, C2D_Color32(255, 255, 255, 255));
    }
};

void App::renderTop()
{
    constexpr uint32_t topBlue = C2D_Color32(2, 167, 225, 255);
    constexpr uint32_t bottomBlue = C2D_Color32(7, 51, 149, 255);
    static GlanceView glance = GlanceView(&assets, "Penistone", 100, 0, false);
    static uint8_t fade = 255;
    static float scale = 0.3;

    if (fade != 0)
    {
        C2D_Fade(C2D_Color32(0, 0, 0, fade -= 5));
    }

    C2D_DrawRectangle(0, 0, 0, Screen::TOP_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, topBlue, topBlue, bottomBlue, bottomBlue);

    if (input.kHeld & KEY_A)
    {
        scale += 0.001;
    }
    else if (input.kHeld & KEY_B)
    {
        scale -= 0.001;
    }

    glance.draw(scale);

    static uint8_t PAGE_SWITCH_BUTTONS_OFFSET = 6;

    C2D_DrawText(&assets.staticText.L_previous, C2D_AlignLeft | C2D_AtBaseline | C2D_WithColor, PAGE_SWITCH_BUTTONS_OFFSET, Screen::SCREEN_HEIGHT - PAGE_SWITCH_BUTTONS_OFFSET, 0, 0.6, 0.6, whiteColor);
    C2D_DrawText(&assets.staticText.R_next, C2D_AlignRight | C2D_AtBaseline | C2D_WithColor, Screen::TOP_SCREEN_WIDTH - PAGE_SWITCH_BUTTONS_OFFSET, Screen::SCREEN_HEIGHT - PAGE_SWITCH_BUTTONS_OFFSET, 0, 0.6, 0.6, whiteColor);
}

static SpriteTextRenderer<6> getBar(Assets &assets)
{
    SpriteTextRenderer<6> a;
    parseTemperature(assets, a, 10, true);
    return a;
}

static SpriteTextRenderer<1> getIcon(Assets &assets)
{
    SpriteTextRenderer<1> a;
    a.addChar(C2D_SpriteSheetGetImage(assets.icons, 0));
    return a;
}

void App::renderBottom()
{
    constexpr uint8_t BARS = 4;
    constexpr uint16_t WEEKDAY_WIDTH = Screen::BOTTOM_SCREEN_WIDTH / BARS;
    constexpr uint8_t TOP_BAR_HEIGHT = 20;
    constexpr uint8_t BOTTOM_BAR_HEIGHT = 30;
    static C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 255), 0);

    static SpriteTextRenderer<6> stuff = getBar(assets);
    static SpriteTextRenderer<1> stuff2 = getIcon(assets);

    C2D_DrawRectSolid(0, 0, 0, Screen::BOTTOM_SCREEN_WIDTH, TOP_BAR_HEIGHT, C2D_Color32(7, 51, 149, 255));
    
    for (uint8_t i = 0; i < BARS; i++)
    {
        uint32_t color = C2D_Color32(10 * (i + 1), 10 * (i + 1), 10 * (i + 1), 255);
        C2D_DrawRectSolid(WEEKDAY_WIDTH * i, TOP_BAR_HEIGHT, 0, WEEKDAY_WIDTH, 300, color);
        stuff.render(WEEKDAY_WIDTH * (i+0.5) , 120, 0.1, &tint);
        stuff2.render(WEEKDAY_WIDTH * (i+0.5) , 60, 0.5, &tint);
    }
    
    C2D_DrawRectSolid(0, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT, 0, Screen::BOTTOM_SCREEN_WIDTH, BOTTOM_BAR_HEIGHT, C2D_Color32(7, 51, 149, 255));
    C2D_DrawText(&assets.staticText.weeklyWeather, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, TOP_BAR_HEIGHT * 0.7 + 2, 0, 0.6, 0.6, whiteColor);
    C2D_DrawText(&assets.staticText.home, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT * 0.3, 0, 0.6, 0.6, whiteColor);
}