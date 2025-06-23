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

class App
{
public:
private:
    bool running = true;

    Screen screen;
    InputHandler input;
    Assets assets;
    WeatherDataLoader weatherData;
    HttpRequestWorker httpWorker;

public:
    App();
    ~App();
    bool frame();

private:
    void beforeRender();
    void render();
    void renderTop();
    void renderBottom();
};