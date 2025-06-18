#pragma once

#include <3ds.h>
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <math.h>

#include "./interactions.h"
#include "./assets.h"

class App
{
public:
private:
    bool running = true;

    Screen screen;
    InputHandler input;
    Assets assets;

    static constexpr uint32_t whiteColor = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
    static constexpr uint32_t clearColor = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
    static constexpr uint32_t greenColor = C2D_Color32(0x18, 0x6b, 0x25, 0xFF);
    
public:
    App();
    ~App();
    bool frame();

private:
    void init();
    void deinit();

    void beforeRender();
    void render();
    void renderTop();
    void renderBottom();
};