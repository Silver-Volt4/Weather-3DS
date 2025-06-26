#pragma once

#include <string>
#include <vector>
#include <optional>

#include "app.h"
#include "../data/weather_data.h"

class App;
class ForecastView;

struct CityWeather
{
    std::string cityName;
    std::optional<WeatherData> weatherData;
    HttpRequest *fetch = nullptr;

    bool good();
};

class GlanceView
{
    ForecastView *forecastView;

    struct Texts : TextRenderer<256>
    {
        C2D_Text cityName;
        C2D_Text weatherState;

        void set(const char *_cityName, const char *_weatherState)
        {
            clearBuffer();
            if (_cityName)
                configureText(&cityName, _cityName);
            if (_weatherState)
                configureText(&weatherState, _weatherState);
        }
    } texts;

    CityWeather *cachedWeatherState = nullptr;
    SpriteTextRenderer<8> str;

public:
    GlanceView(ForecastView *forecastView);
    void rebuild();
    void render();
};

class ForecastView
{
    friend GlanceView;

    App *app;
    std::vector<CityWeather *> pages;
    GlanceView glance;

public:
    ForecastView(App *app);

    CityWeather *currentPage = nullptr;
    bool celsius = true;

    void poll();
    void renderTop();
    void renderBottom();
    void updateWeather();
};