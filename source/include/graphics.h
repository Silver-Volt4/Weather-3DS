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
    static constexpr float gap = 30;
    static constexpr float space = 60;

    struct
    {
        float scale = 1.0;
        C2D_Image image;
    } images[Capacity];

    uint8_t textLength = 0;

    bool sizeCached = false;
    float _scale = 1.0;
    float width;
    float height;

    void cacheSize()
    {
        if (sizeCached)
            return;

        width = 0.0;
        height = 0.0;
        for (uint8_t i = 0; i < textLength; i++)
        {
            bool last = i + 1 == textLength;
            auto frame = images[i];
            if (frame.scale == 0)
            {
                width += (space)*_scale;
            }
            else
            {
                width += (frame.image.subtex->width + (last ? 0 : gap)) * _scale * frame.scale;
                height = std::max(height, frame.image.subtex->height * _scale * frame.scale);
            }
        }
        sizeCached = true;
    }

public:
    float render(float xPos, float yPos, C2D_ImageTint *tint)
    {
        return render(xPos, yPos, tint, textLength - 1);
    }

    float render(float xPos, float yPos, C2D_ImageTint *tint, uint8_t capturedPosition)
    {
        cacheSize();

        float capture = 0;

        xPos -= width / 2;
        yPos -= height / 2;

        for (uint8_t i = 0; i < textLength; i++)
        {
            bool last = i + 1 == textLength;
            auto frame = images[i];
            if (frame.scale == 0)
            {
                xPos += (space)*_scale;
            }
            else
            {
                C2D_DrawImageAt(frame.image, xPos, yPos + (height - frame.image.subtex->height * _scale * frame.scale) / 2, 0, tint, _scale * frame.scale, _scale * frame.scale);
                xPos += (frame.image.subtex->width + (last ? 0 : gap)) * _scale * frame.scale;
            }

            if (capturedPosition == i)
            {
                capture = xPos;
            }
        }

        return capture;
    }

    void clear()
    {
        textLength = 0;
        sizeCached = false;
    }

    void addChar(C2D_Image image, float scale = 1.0)
    {
        images[textLength++] = {scale, image};
        sizeCached = false;
    }

    void addSpace()
    {
        images[textLength++].scale = 0;
        sizeCached = false;
    }

    void scale(float newScale)
    {
        _scale = newScale;
        sizeCached = false;
    }
};