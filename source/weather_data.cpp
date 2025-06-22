#include "./include/weather_data.h"

#include "libs/json.hpp"

#include <3ds.h>
#include <stdio.h>
#include <array>

using json = nlohmann::json;

WeatherData WeatherDataLoader::fetch()
{
    Result ret = 0;
    httpcContext context;
    char *newurl = NULL;
    u8 *framebuf_top;
    u32 statuscode = 0;
    u32 contentsize = 0, readsize = 0, size = 0;
    u8 *buf, *lastbuf;

    char base[300];

    // TODO: ugly alloc stuff
    // TODO: percent escape
    auto r = snprintf(base, 300, "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", "Vilnius", API_KEY);
    base[r] = '\0';

    printf("%s\n", base);

    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, base, 1);

    // This disables SSL cert verification, so https:// will be usable
    ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

    // Enable Keep-Alive connections
    ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);

    // Set a User-Agent header so websites can identify your application
    ret = httpcAddRequestHeaderField(&context, "User-Agent", "httpc-example/1.0.0");

    // Tell the server we can support Keep-Alive connections.
    // This will delay connection teardown momentarily (typically 5s)
    // in case there is another request made to the same server.
    ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

    ret = httpcBeginRequest(&context);

    ret = httpcGetResponseStatusCode(&context, &statuscode);

    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);

    printf("reported size: %d\n", contentsize);

    uint8_t *content = new uint8_t[contentsize + 1];

    ret = httpcDownloadData(&context, content, contentsize, &readsize);

    content[readsize] = '\0';

    httpcCloseContext(&context);

    printf("Status was %d\n", statuscode);

    json j = json::parse(content, nullptr, false);
    delete[] content;

    WeatherData w;

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
            if (iconName.rfind("01", 0))
            {
                w.icon = Assets::WeatherIcons::CLEAR_SKY;
            }
            else if (iconName.rfind("02", 0))
            {
                w.icon = Assets::WeatherIcons::CLOUDY;
            }
            else if (iconName.rfind("03", 0))
            {
                w.icon = Assets::WeatherIcons::PARTLY_CLOUDY;
            }
            else if (iconName.rfind("04", 0))
            {
                w.icon = Assets::WeatherIcons::CLOUDY;
            }
            else if (iconName.rfind("09", 0))
            {
                w.icon = Assets::WeatherIcons::RAIN;
            }
            else if (iconName.rfind("10", 0))
            {
                w.icon = Assets::WeatherIcons::RAIN;
            }
            else if (iconName.rfind("11", 0))
            {
                w.icon = Assets::WeatherIcons::STORMS;
            }
            else if (iconName.rfind("13", 0))
            {
                w.icon = Assets::WeatherIcons::SNOW;
            }
            else if (iconName.rfind("50", 0))
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