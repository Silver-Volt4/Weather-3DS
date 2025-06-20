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
