#pragma once

#include <3ds.h>
#include <citro2d.h>

#include "./graphics.h"

struct Assets
{
    C2D_SpriteSheet numbers;
    C2D_SpriteSheet icons;
    C2D_SpriteSheet gui;

    struct StaticText : TextRenderer<1024>
    {
        C2D_Text weeklyWeather;
        C2D_Text home;
        C2D_Text L_previous;
        C2D_Text R_next;

        StaticText()
        {
            configureText(&weeklyWeather, "This week's forecast");
            configureText(&home, "Press \uE073 to return to the Home menu.");
            configureText(&L_previous, "\uE004 Previous");
            configureText(&R_next, "Next \uE005");
        }
    } staticText;

    void init();
    void deinit();
};