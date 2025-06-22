#pragma once

#ifndef OWM_API_KEY
// suppress editor errors
#define OWM_API_KEY ""
#error "OpenWeatherMap API key is missing, specify it in .env file"
#endif

#include "./assets.h"
#include <string>

struct WeatherData
{
    Assets::WeatherIcons icon;
    float tempKelvin;
    std::string state;
};

class WeatherDataLoader
{
public:
    static constexpr char *API_KEY = OWM_API_KEY;
    WeatherData fetch();
};