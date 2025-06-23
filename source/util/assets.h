#pragma once

#include <3ds.h>
#include <citro2d.h>

#include "graphics.h"

struct Assets
{
    enum WeatherIcons
    {
        CLEAR_SKY = 0,     // icons/clear.png
        CLOUDY = 1,        // icons/cloudy.png
        PARTLY_CLOUDY = 2, // icons/partlycloudy.png
        RAIN = 3,          // icons/rain.png
        SNOW = 4,          // icons/snow.png
        STORMS = 5         // icons/thunderstorms.png
    };

    C2D_SpriteSheet numbers;
    C2D_SpriteSheet icons;

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