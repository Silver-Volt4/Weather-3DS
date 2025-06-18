#include "include/app.h"

struct Label
{
    C2D_TextBuf buffer;
    C2D_Text text;

    Label(const char *string, int bufsize)
    {
        buffer = C2D_TextBufNew(bufsize);
        C2D_TextParse(&text, buffer, string);
        text.width = 10;
        C2D_TextOptimize(&text);
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
    C2D_Sprite weatherIcon;
    uint8_t textIndices[6];
    uint8_t textLength;

    void renderBitmapText(float xPos, float yPos, float scale, C2D_ImageTint *tint)
    {
        constexpr float gap = 30;
        constexpr float space = 60;
        float width = 0;
        float height = 0;

        for (uint8_t i = 0; i < textLength; i++)
        {
            uint8_t index = textIndices[i];
            if (index == 255)
            {
                width += (space + gap) * scale;
            }
            else
            {
                auto image = C2D_SpriteSheetGetImage(assets->numbers, index);
                width += (image.subtex->width + gap) * scale;
                height = std::max(height, image.subtex->height * scale);
            }
        }

        xPos -= width / 2;
        yPos -= height / 2;

        for (uint8_t i = 0; i < textLength; i++)
        {
            uint8_t index = textIndices[i];
            if (index == 255)
            {
                xPos += (space + gap) * scale;
            }
            else
            {
                auto image = C2D_SpriteSheetGetImage(assets->numbers, index);
                C2D_DrawImageAt(image, xPos, yPos, 0, tint, scale, scale);
                xPos += (image.subtex->width + gap) * scale;
            }
        }
    }

public:
    GlanceView(Assets *assets, int8_t tempValue, uint8_t weatherState, bool celsius) : assets(assets)
    {
        C2D_SpriteFromSheet(&weatherIcon, assets->icons, weatherState);

        textLength = 0;
        if (tempValue < 0)
        {
            textIndices[textLength++] = 11; // index of -
            tempValue = std::abs(tempValue);
        }

        bool leading = true;
        for (uint8_t order = 100; order > 0; order /= 10)
        {
            uint8_t digit = tempValue / order;
            if (digit || !leading)
            {
                leading = false;
                textIndices[textLength++] = digit;
            }
            tempValue -= digit * order;
        }

        if (celsius)
        {
            textIndices[textLength++] = 10; // index of °
            textIndices[textLength++] = 12; // index of C
        }
        else
        {
            textIndices[textLength++] = 255; // space
            textIndices[textLength++] = 13;  // index of F
        }

        constexpr float GLANCE_ICON_X = Screen::TOP_SCREEN_WIDTH / 2 - Screen::TOP_SCREEN_WIDTH / 3.5;
        constexpr float GLANCE_ICON_Y = Screen::SCREEN_HEIGHT / 2;
        C2D_SpriteSetPos(&weatherIcon, GLANCE_ICON_X, GLANCE_ICON_Y);
        C2D_SpriteSetScale(&weatherIcon, 0.8, 0.8);
        C2D_SpriteSetCenter(&weatherIcon, 0.5, 0.5);
    }

    void draw()
    {
        static C2D_ImageTint tint;

        constexpr float GLANCE_TEXT_X = Screen::TOP_SCREEN_WIDTH / 6 + Screen::TOP_SCREEN_WIDTH / 2;
        constexpr float GLANCE_TEXT_Y = Screen::SCREEN_HEIGHT / 2;

        C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 0, 100), 1);
        renderBitmapText(GLANCE_TEXT_X + 4, GLANCE_TEXT_Y + 4, 0.3, &tint);
        C2D_SpriteMove(&weatherIcon, +4, +4);
        C2D_DrawSpriteTinted(&weatherIcon, &tint);

        C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 255), 1);
        renderBitmapText(GLANCE_TEXT_X, GLANCE_TEXT_Y, 0.3, &tint);
        C2D_SpriteMove(&weatherIcon, -4, -4);
        C2D_DrawSprite(&weatherIcon);
    }
};

void App::renderTop()
{
    constexpr u32 topBlue = C2D_Color32(2, 167, 225, 255);
    constexpr u32 bottomBlue = C2D_Color32(7, 51, 149, 255);
    static GlanceView glance = GlanceView(&assets, 30, 0, true);

    C2D_DrawRectangle(0, 0, 0, Screen::TOP_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, topBlue, topBlue, bottomBlue, bottomBlue);
    glance.draw();
}

void App::renderBottom()
{
    static Label labels[2] = {
        Label("Test", 32),
        Label("Druhý test", 32)};

    C2D_DrawText(&labels[1].text, C2D_AlignCenter | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, 3, 0, 0.6, 0.6, whiteColor);
}