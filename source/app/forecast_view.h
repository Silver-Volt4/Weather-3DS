#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "../data/weather_data.h"

class App;
class ForecastView;

struct CityWeather
{
    std::string cityName;
    std::optional<WeatherData> weatherData;
    std::shared_ptr<HttpRequest> fetch = nullptr;

    bool good();
};

class ActionButton
{
    ForecastView *parent;
    bool down = false;

public:
    ActionButton(ForecastView *parent);

    bool render(float x, float y, float w, float h, C2D_Text &text);
};

class GlanceView
{
    ForecastView *parent;

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
    GlanceView(ForecastView *parent);
    void rebuild();
    void render();
};

class ForecastView
{
    friend GlanceView;

    std::vector<CityWeather *> pages;

public:
    App *parent;

private:
    GlanceView glance;
    ActionButton settingsButton;

public:
    ForecastView(App *parent);
    ~ForecastView();

    CityWeather *currentPage = nullptr;
    bool celsius = true;

    void poll();
    void renderTop();
    void renderBottom();
    void updateWeather();
};