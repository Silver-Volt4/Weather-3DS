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
};

class ForecastView
{
    std::vector<CityWeather *> pages;

public:
    void renderTop(App &app);
    void renderBottom(App &app);
};