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
    struct Texts : TextRenderer<256>
    {
        C2D_Text cityName;
        C2D_Text weatherState;

        void setCityName(std::string name)
        {
            clearBuffer();
            configureText(&cityName, name.c_str());
            configureText(&weatherState, "Sunny");
        }
    } texts;

    SpriteTextRenderer<8> str;

public:
    GlanceView(Assets *assets, std::string name, int8_t tempValue, uint8_t weatherState, bool celsius) : assets(assets)
    {
        texts.setCityName(name);

        str.addChar(C2D_SpriteSheetGetImage(assets->icons, weatherState), 2.0);
        parseTemperature(*assets, str, tempValue, celsius);
        str.scale(0.3);
    }

    void draw()
    {
        static C2D_ImageTint tint;

        constexpr float GLANCE_TEXT_X = Screen::TOP_SCREEN_WIDTH / 2;
        constexpr float GLANCE_TEXT_Y = Screen::SCREEN_HEIGHT / 2;

        constexpr uint32_t WHITE = C2D_Color32(255, 255, 255, 255);
        constexpr uint32_t SHADOW = C2D_Color32(0, 0, 0, 100);

        C2D_PlainImageTint(&tint, SHADOW, 1);
        str.render(GLANCE_TEXT_X + (10 * 0.3), GLANCE_TEXT_Y + (10 * 0.3), &tint);

        C2D_PlainImageTint(&tint, WHITE, 0);
        float a = str.render(GLANCE_TEXT_X, GLANCE_TEXT_Y, &tint, 0);

        C2D_DrawImageAt(C2D_SpriteSheetGetImage(assets->gui, 0), 40, 0, 0);
        C2D_DrawText(&texts.cityName, C2D_AlignCenter | C2D_WithColor, 200, 6, 0, 0.8, 0.8, WHITE);

        C2D_DrawText(&texts.weatherState, C2D_AlignLeft | C2D_WithColor, a + (8 * 0.3), 50 + (8 * 0.3), 0, 1, 1, SHADOW);
        C2D_DrawText(&texts.weatherState, C2D_AlignLeft | C2D_WithColor, a, 50, 0, 1, 1, WHITE);
    }
};

void App::renderTop()
{
    constexpr uint32_t topBlue = C2D_Color32(2, 167, 225, 255);
    constexpr uint32_t bottomBlue = C2D_Color32(7, 51, 149, 255);
    static GlanceView glance = GlanceView(&assets, "Penistone", 100, 0, false);
    static uint8_t fade = 255;

    if (fade != 0)
    {
        C2D_Fade(C2D_Color32(0, 0, 0, fade -= 5));
    }

    C2D_DrawRectangle(0, 0, 0, Screen::TOP_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, topBlue, topBlue, bottomBlue, bottomBlue);

    glance.draw();

    static uint8_t PAGE_SWITCH_BUTTONS_OFFSET = 6;

    C2D_DrawText(&assets.staticText.L_previous, C2D_AlignLeft | C2D_AtBaseline | C2D_WithColor, PAGE_SWITCH_BUTTONS_OFFSET, Screen::SCREEN_HEIGHT - PAGE_SWITCH_BUTTONS_OFFSET, 0, 0.6, 0.6, whiteColor);
    C2D_DrawText(&assets.staticText.R_next, C2D_AlignRight | C2D_AtBaseline | C2D_WithColor, Screen::TOP_SCREEN_WIDTH - PAGE_SWITCH_BUTTONS_OFFSET, Screen::SCREEN_HEIGHT - PAGE_SWITCH_BUTTONS_OFFSET, 0, 0.6, 0.6, whiteColor);
}

static SpriteTextRenderer<6> getBar(Assets &assets)
{
    SpriteTextRenderer<6> a;
    parseTemperature(assets, a, 10, true);
    a.scale(0.1);
    return a;
}

static SpriteTextRenderer<1> getIcon(Assets &assets)
{
    SpriteTextRenderer<1> a;
    a.addChar(C2D_SpriteSheetGetImage(assets.icons, 0));
    a.scale(0.5);
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
        stuff.render(WEEKDAY_WIDTH * (i + 0.5), 120, &tint);
        stuff2.render(WEEKDAY_WIDTH * (i + 0.5), 60, &tint);
    }

    C2D_DrawRectSolid(0, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT, 0, Screen::BOTTOM_SCREEN_WIDTH, BOTTOM_BAR_HEIGHT, C2D_Color32(7, 51, 149, 255));
    C2D_DrawText(&assets.staticText.weeklyWeather, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, TOP_BAR_HEIGHT * 0.7 + 2, 0, 0.6, 0.6, whiteColor);
    C2D_DrawText(&assets.staticText.home, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT * 0.3, 0, 0.6, 0.6, whiteColor);
}