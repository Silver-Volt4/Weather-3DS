#include "weather_data.h"

#include <3ds.h>
#include <stdio.h>
#include <array>

#include "../libs/json.hpp"
#include "../util/http.h"

using json = nlohmann::json;

httpcContext WeatherDataLoader::requestWeatherFor(std::string &cityName)
{
    Result ret = 0;
    httpcContext context;

    std::string addr = "http://api.openweathermap.org/data/2.5/weather?q=" + cityName + "&appid=" + WeatherDataLoader::API_KEY;

    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, addr.c_str(), 1);
    ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
    ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
    ret = httpcAddRequestHeaderField(&context, "User-Agent", "3ds-weather");
    ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

    return context;
}

WeatherData WeatherDataLoader::parseResponse(HttpRequest &request)
{
    WeatherData w;
    json j = json::parse(request.result, nullptr, false);

    if (j.is_discarded())
    {
        printf("parse error!");
    }
    else
    {
        auto temp = j["main"]["temp"];
        auto icon = j["weather"][0]["icon"];
        auto state = j["weather"][0]["main"];

        if (temp.is_number())
        {
            w.tempKelvin = temp.get<float>();
        }

        if (icon.is_string())
        {
            std::string iconName = icon.get<std::string>();
            if (iconName.rfind("01", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::CLEAR_SKY;
            }
            else if (iconName.rfind("02", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::CLOUDY;
            }
            else if (iconName.rfind("03", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::PARTLY_CLOUDY;
            }
            else if (iconName.rfind("04", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::CLOUDY;
            }
            else if (iconName.rfind("09", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::RAIN;
            }
            else if (iconName.rfind("10", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::RAIN;
            }
            else if (iconName.rfind("11", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::STORMS;
            }
            else if (iconName.rfind("13", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::SNOW;
            }
            else if (iconName.rfind("50", 0) != std::string::npos)
            {
                w.icon = Assets::WeatherIcons::SNOW;
            }
        }

        if (state.is_string())
        {
            w.state = state.get<std::string>();
        }
    }

    return w;
}