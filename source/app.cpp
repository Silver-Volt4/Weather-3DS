#include "include/app.h"

template <int BufSize>
class TextRenderer
{
protected:
    C2D_TextBuf buffer;

    void clearBuffer()
    {
        C2D_TextBufClear(buffer);
    }

    void configureText(C2D_Text *text, const char *string)
    {
        C2D_TextParse(text, buffer, string);
        C2D_TextOptimize(text);
    }

public:
    TextRenderer()
    {
        buffer = C2D_TextBufNew(BufSize);
    }

    ~TextRenderer()
    {
        C2D_TextBufDelete(buffer);
    }
};

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

    struct
    {
        float scale = 1.0;
        C2D_Image image;
    } images[8];
    uint8_t textLength;

    void renderGlanceView(float xPos, float yPos, float scale, C2D_ImageTint *tint)
    {
        constexpr float gap = 30;
        constexpr float space = 60;
        float width = 0;
        float height = 0;

        for (uint8_t i = 0; i < textLength; i++)
        {
            auto frame = images[i];
            if (frame.scale == 0)
            {
                width += (space)*scale;
            }
            else
            {
                width += (frame.image.subtex->width + gap) * scale * frame.scale;
                height = std::max(height, frame.image.subtex->height * scale * frame.scale);
            }
        }

        xPos -= width / 2;
        yPos -= height / 2;

        for (uint8_t i = 0; i < textLength; i++)
        {
            auto frame = images[i];
            if (frame.scale == 0)
            {
                xPos += (space)*scale;
            }
            else
            {
                C2D_DrawImageAt(frame.image, xPos, yPos + (height - frame.image.subtex->height * scale * frame.scale) / 2, 0, tint, scale * frame.scale, scale * frame.scale);
                xPos += (frame.image.subtex->width + gap) * scale * frame.scale;
            }
        }
    }

public:
    GlanceView(Assets *assets, std::string name, int8_t tempValue, uint8_t weatherState, bool celsius) : assets(assets)
    {
        texts.setCityName(name);

        textLength = 0;

        images[textLength++] = {2.0, C2D_SpriteSheetGetImage(assets->icons, weatherState)};
        images[textLength++].scale = 0;

        if (tempValue < 0)
        {
            images[textLength++].image = C2D_SpriteSheetGetImage(assets->numbers, 11); // minus
            tempValue = std::abs(tempValue);
        }

        bool leading = true;
        for (uint8_t order = 100; order > 0; order /= 10)
        {
            uint8_t digit = tempValue / order;
            if (digit || !leading)
            {
                leading = false;
                images[textLength++].image = C2D_SpriteSheetGetImage(assets->numbers, digit);
            }
            tempValue -= digit * order;
        }

        if (celsius)
        {
            images[textLength++].image = C2D_SpriteSheetGetImage(assets->numbers, 10); // °
            images[textLength++].image = C2D_SpriteSheetGetImage(assets->numbers, 12); // C
        }
        else
        {
            images[textLength++].scale = 0;
            images[textLength++].image = C2D_SpriteSheetGetImage(assets->numbers, 13); // index of F
        }
    }

    void draw(float scale)
    {
        static C2D_ImageTint tint;

        constexpr float GLANCE_TEXT_X = Screen::TOP_SCREEN_WIDTH / 2;
        constexpr float GLANCE_TEXT_Y = Screen::SCREEN_HEIGHT / 2;

        C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 0, 100), 1);
        renderGlanceView(GLANCE_TEXT_X + (10 * scale), GLANCE_TEXT_Y + (10 * scale), scale, &tint);

        C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 255), 0);
        renderGlanceView(GLANCE_TEXT_X, GLANCE_TEXT_Y, scale, &tint);

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
}

void App::renderBottom()
{
    static struct StaticText : TextRenderer<1024>
    {
        C2D_Text weeklyWeather;
        C2D_Text home;

        StaticText()
        {
            configureText(&weeklyWeather, "This week's forecast");
            configureText(&home, "Press \uE073 to return to the Home menu.");
        }
    } texts;

    constexpr uint16_t WEEKDAY_WIDTH = Screen::BOTTOM_SCREEN_WIDTH / 4;
    constexpr uint8_t TOP_BAR_HEIGHT = 20;
    constexpr uint8_t BOTTOM_BAR_HEIGHT = 30;

    C2D_DrawRectSolid(0, 0, 0, Screen::BOTTOM_SCREEN_WIDTH, TOP_BAR_HEIGHT, C2D_Color32(7, 51, 149, 255));
    C2D_DrawRectSolid(0, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT, 0, Screen::BOTTOM_SCREEN_WIDTH, BOTTOM_BAR_HEIGHT, C2D_Color32(7, 51, 149, 255));

    for (uint8_t i = 0; i < 4; i++)
    {
        uint32_t color = C2D_Color32(10 * (i+1), 10 * (i+1), 10 * (i+1), 255);
        C2D_DrawRectSolid(WEEKDAY_WIDTH * i, TOP_BAR_HEIGHT, 0, WEEKDAY_WIDTH, 100, color);
    }

    C2D_DrawText(&texts.weeklyWeather, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, TOP_BAR_HEIGHT * 0.7 + 2, 0, 0.6, 0.6, whiteColor);
    C2D_DrawText(&texts.home, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT * 0.3, 0, 0.6, 0.6, whiteColor);
}