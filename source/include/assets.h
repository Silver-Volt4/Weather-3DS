#pragma once

#include <3ds.h>
#include <citro2d.h>

struct Assets
{
    C2D_SpriteSheet numbers;
    C2D_SpriteSheet icons;
    C2D_SpriteSheet gui;

    void init();
    void deinit();
};