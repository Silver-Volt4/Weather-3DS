#pragma once

#include <3ds.h>
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <vector>
#include <random>
#include <algorithm>
#include <memory>

#include "../util/interactions.h"
#include "../util/assets.h"
#include "../util/http.h"
#include "../data/weather_data.h"

#include "forecast_view.h"
#include "settings_view.h"

class App
{
    friend ForecastView;
    friend SettingsView;

    struct Views
    {
        ForecastView forecast;
        SettingsView settings;

        enum Types
        {
            NIL,
            FORECAST,
            SETTINGS
        };

        uint8_t fade = 255;
        App::Views::Types current = FORECAST;
        App::Views::Types queuedUp = NIL;

        Views(App *app) : forecast(app), settings(app) {};
    } views;

    bool running = true;

    void beforeRender();
    void render();

public:
    Screen screen;
    InputHandler input;
    Assets assets;
    HttpRequestWorker httpWorker;

    App();
    ~App();
    bool frame();
    void changeView(App::Views::Types view);
};