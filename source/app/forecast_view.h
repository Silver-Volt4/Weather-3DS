#pragma once

#include <string>
#include <vector>

#include "app.h"
#include "../data/weather_data.h"

class App;

struct CityWeather
{
    std::string cityName;
    WeatherData weatherData;
    HttpRequest *fetch = nullptr;
};

class GlanceView
{
    struct Texts : TextRenderer<256>
    {
        C2D_Text cityName;
        C2D_Text weatherState;

        void set(const char *_cityName, const char *_weatherState)
        {
            clearBuffer();
            configureText(&cityName, _cityName);
            configureText(&weatherState, _weatherState);
        }
    } texts;

    bool valid = false;
    SpriteTextRenderer<8> str;

public:
    void set(Assets &assets, const CityWeather &cityWeather, bool celsius);
    void render();
};

class ForecastView
{
    CityWeather *currentPage = nullptr;
    std::vector<CityWeather *> pages;

    GlanceView glance;
    bool celsius = true;

public:
    ForecastView();

    void poll(App &app);
    void renderTop(App &app);
    void renderBottom(App &app);
    void updateWeather(App &app);
};