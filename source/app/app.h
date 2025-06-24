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

class App
{
    friend ForecastView;
    
    struct Views
    {
        ForecastView forecast;
        
        enum
        {
            FORECAST
        } current = FORECAST;
        
        Views(App *app) : forecast(app) {};
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
};