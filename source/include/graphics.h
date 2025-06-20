#pragma once

#include <citro2d.h>

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

template <uint8_t Capacity>
class SpriteTextRenderer
{
    struct
    {
        float scale = 1.0;
        C2D_Image image;
    } images[Capacity];
    uint8_t textLength = 0;

public:
    void render(float xPos, float yPos, float scale, C2D_ImageTint *tint)
    {
        constexpr float gap = 30;
        constexpr float space = 60;
        float width = 0;
        float height = 0;

        for (uint8_t i = 0; i < textLength; i++)
        {
            bool last = i + 1 == textLength;
            auto frame = images[i];
            if (frame.scale == 0)
            {
                width += (space)*scale;
            }
            else
            {
                width += (frame.image.subtex->width + (last ? 0 : gap)) * scale * frame.scale;
                height = std::max(height, frame.image.subtex->height * scale * frame.scale);
            }
        }

        xPos -= width / 2;
        yPos -= height / 2;

        for (uint8_t i = 0; i < textLength; i++)
        {
            bool last = i + 1 == textLength;
            auto frame = images[i];
            if (frame.scale == 0)
            {
                xPos += (space)*scale;
            }
            else
            {
                C2D_DrawImageAt(frame.image, xPos, yPos + (height - frame.image.subtex->height * scale * frame.scale) / 2, 0, tint, scale * frame.scale, scale * frame.scale);
                xPos += (frame.image.subtex->width + (last ? 0 : gap)) * scale * frame.scale;
            }
        }
    }

    void clear()
    {
        textLength = 0;
    }

    void addChar(C2D_Image image, float scale = 1.0)
    {
        images[textLength++] = {scale, image};
    }

    void addSpace()
    {
        images[textLength++].scale = 0;
    }
};