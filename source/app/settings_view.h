#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../data/weather_data.h"

class App;

class SettingsView
{
public:
    App *parent;
    SettingsView(App *parent);

    void poll();
    void renderTop();
    void renderBottom();
};