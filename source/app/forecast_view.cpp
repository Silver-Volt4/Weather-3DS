#include "app.h"

#include "../util/graphics.h"
#include "../data/weather_data.h"

constexpr uint32_t WHITE = C2D_Color32(255, 255, 255, 255);
constexpr uint32_t SHADOW = C2D_Color32(0, 0, 0, 100);
constexpr uint32_t BLUE_GRADIENT_TOP = C2D_Color32(2, 167, 225, 255);
constexpr uint32_t BLUE_GRADIENT_BOTTOM = C2D_Color32(7, 51, 149, 255);

bool CityWeather::good()
{
    return fetch == nullptr && weatherData.has_value();
}

template <uint8_t S>
void parseTemperature(Assets &assets, SpriteTextRenderer<S> &str, int8_t tempValue, bool celsius)
{
    if (tempValue < 0)
    {
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 11)); // minus
        tempValue = std::abs(tempValue);
    }

    bool leading = true;
    for (uint8_t order = 100; order > 0; order /= 10)
    {
        uint8_t digit = tempValue / order;
        if (digit || !leading)
        {
            leading = false;
            str.addChar(C2D_SpriteSheetGetImage(assets.numbers, digit));
        }
        tempValue -= digit * order;
    }

    if (celsius)
    {
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 10)); // °
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 12)); // C
    }
    else
    {
        str.addSpace();
        str.addChar(C2D_SpriteSheetGetImage(assets.numbers, 13)); // index of F
    }
}

void drawGlossy(float x, float y, float z, float w, float h)
{
    constexpr uint32_t GLOSS_GRAD_START = C2D_Color32(200, 200, 200, 100);
    constexpr uint32_t GLOSS_GRAD_MID = C2D_Color32(0, 0, 0, 100);
    constexpr uint32_t GLOSS_GRAD_END = C2D_Color32(0, 0, 0, 150);

    C2D_DrawRectangle(x, y, z, w, h / 2, GLOSS_GRAD_START, GLOSS_GRAD_START, GLOSS_GRAD_MID, GLOSS_GRAD_MID);
    C2D_DrawRectangle(0, h / 2, z, w, h / 2, GLOSS_GRAD_MID, GLOSS_GRAD_MID, GLOSS_GRAD_END, GLOSS_GRAD_END);
}

GlanceView::GlanceView(ForecastView *forecastView) : parent(forecastView)
{
}

// Recalculate inner state
void GlanceView::rebuild()
{
    const char *weatherState = nullptr;

    if (parent->currentPage->weatherData.has_value())
    {
        weatherState = parent->currentPage->weatherData->state.c_str();
        str.clear();

        str.addChar(C2D_SpriteSheetGetImage(parent->parent->assets.icons, (uint8_t)parent->currentPage->weatherData->icon), 2.0);
        parseTemperature(parent->parent->assets, str, parent->currentPage->weatherData->tempKelvin - 273.5, parent->celsius);
        str.scale(0.3);
    }

    texts.set(parent->currentPage->cityName.c_str(), weatherState);

    cachedWeatherState = parent->currentPage;
}

void GlanceView::render()
{
    static C2D_ImageTint tint;

    constexpr float GLANCE_TEXT_X = Screen::TOP_SCREEN_WIDTH / 2;
    constexpr float GLANCE_TEXT_Y = Screen::SCREEN_HEIGHT / 2 + 10;

    constexpr uint8_t TOP_BAR_HEIGHT = 30;

    if (!parent->currentPage)
    {
        return;
    }

    if (parent->currentPage != cachedWeatherState)
    {
        rebuild();
    }

    // Draw header
    drawGlossy(0, 0, 0, Screen::TOP_SCREEN_WIDTH, TOP_BAR_HEIGHT);
    C2D_DrawText(&texts.cityName, C2D_AlignCenter | C2D_WithColor, Screen::TOP_SCREEN_WIDTH / 2, (30 - 30 * 0.8) / 2, 0, 0.8, 0.8, WHITE);

    if (!parent->currentPage->good())
    {
        return;
    }

    C2D_PlainImageTint(&tint, SHADOW, 1);
    str.render(GLANCE_TEXT_X + (10 * 0.3), GLANCE_TEXT_Y + (10 * 0.3), &tint);

    C2D_PlainImageTint(&tint, WHITE, 0);
    float numberStartsAt = str.render(GLANCE_TEXT_X, GLANCE_TEXT_Y, &tint, 0);

    C2D_DrawText(&texts.weatherState, C2D_AlignLeft | C2D_WithColor, numberStartsAt + (8 * 0.3), 50 + 10 + (8 * 0.3), 0, 1, 1, SHADOW);
    C2D_DrawText(&texts.weatherState, C2D_AlignLeft | C2D_WithColor, numberStartsAt, 50 + 10, 0, 1, 1, WHITE);
}

ForecastView::ForecastView(App *app) : parent(app), glance(this), settingsButton(this)
{
    // TODO: Free these
    pages.push_back(new CityWeather{"Penistone", {}});
    pages.push_back(currentPage = new CityWeather{"Vilnius", {}});
    pages.push_back(new CityWeather{"Prague", {}});
}

void ForecastView::poll()
{
    static bool init = false;
    if (!init)
    {
        init = true;
        updateWeather();
    }
    for (auto page : pages)
    {
        if (page->fetch && page->fetch->state == HttpRequest::FINISHED)
        {
            page->weatherData = WeatherDataLoader::parseResponse(page->fetch);
            delete page->fetch;
            if (page == currentPage)
            {
                glance.rebuild();
            }
            page->fetch = nullptr;
        }
    }

    if (parent->input.kDown & (KEY_L | KEY_R))
    {
        auto pos = std::find(pages.begin(), pages.end(), currentPage);

        if (parent->input.kDown & KEY_L)
        {
            if (pos != pages.begin())
            {
                pos -= 1;
                currentPage = *pos;
            }
        }
        else
        {
            if (pos != pages.end() && (pos += 1) != pages.end())
            {
                currentPage = *pos;
            }
        }
    }
}

void ForecastView::updateWeather()
{
    for (auto page : pages)
    {
        page->fetch = parent->httpWorker.add(WeatherDataLoader::requestWeatherFor(page->cityName));
    }
}

void ForecastView::renderTop()
{
    C2D_DrawRectangle(0, 0, 0, Screen::TOP_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, BLUE_GRADIENT_TOP, BLUE_GRADIENT_TOP, BLUE_GRADIENT_BOTTOM, BLUE_GRADIENT_BOTTOM);

    glance.render();

    constexpr uint8_t PAGE_SWITCH_BUTTONS_PADDING = 6;

    C2D_DrawText(&parent->assets.staticText.L_previous, C2D_AlignLeft | C2D_AtBaseline | C2D_WithColor, PAGE_SWITCH_BUTTONS_PADDING, Screen::SCREEN_HEIGHT - PAGE_SWITCH_BUTTONS_PADDING, 0, 0.6, 0.6, WHITE);
    C2D_DrawText(&parent->assets.staticText.R_next, C2D_AlignRight | C2D_AtBaseline | C2D_WithColor, Screen::TOP_SCREEN_WIDTH - PAGE_SWITCH_BUTTONS_PADDING, Screen::SCREEN_HEIGHT - PAGE_SWITCH_BUTTONS_PADDING, 0, 0.6, 0.6, WHITE);
}

static SpriteTextRenderer<6> getBar(Assets &assets)
{
    SpriteTextRenderer<6> a;
    parseTemperature(assets, a, 10, true);
    a.scale(0.1);
    return a;
}

static SpriteTextRenderer<1> getIcon(Assets &assets)
{
    SpriteTextRenderer<1> a;
    a.addChar(C2D_SpriteSheetGetImage(assets.icons, 0));
    a.scale(0.5);
    return a;
}

ActionButton::ActionButton(ForecastView *parent) : parent(parent)
{
}

bool ActionButton::render(float x, float y, float w, float h, C2D_Text &text)
{
    auto &touch = parent->parent->input.touchCurrent;
    bool inButtonArea = touch.px > x && touch.px < x + w && touch.py > y && touch.py < y + h;

    if (parent->parent->input.kDown & KEY_TOUCH)
    {
        down = inButtonArea;
    }
    else if (down)
    {
        down = inButtonArea;
    }

    if (down)
    {
        C2D_DrawRectangle(x, y, 0, w, h, BLUE_GRADIENT_BOTTOM, BLUE_GRADIENT_BOTTOM, BLUE_GRADIENT_TOP, BLUE_GRADIENT_TOP);
    }
    else
    {
        C2D_DrawRectangle(x, y, 0, w, h, BLUE_GRADIENT_TOP, BLUE_GRADIENT_TOP, BLUE_GRADIENT_BOTTOM, BLUE_GRADIENT_BOTTOM);
    }

    C2D_DrawText(&text, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, x + w / 2, y + h / 2 + 20*0.4, 0, 0.8, 0.8, WHITE);

    return down && parent->parent->input.kUp & KEY_TOUCH;
}

void ForecastView::renderBottom()
{
    constexpr uint32_t SPLITTER_START = C2D_Color32(0, 0, 0, 100);
    constexpr uint32_t SPLITTER_END = C2D_Color32(200, 200, 200, 100);
    constexpr uint8_t SPLITTER_MARGIN = 10;
    constexpr uint8_t BARS = 4;
    constexpr uint16_t WEEKDAY_WIDTH = Screen::BOTTOM_SCREEN_WIDTH / BARS;
    constexpr uint8_t TOP_BAR_HEIGHT = 20;
    constexpr uint8_t BOTTOM_BAR_HEIGHT = 30;

    static C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 255), 0);

    static SpriteTextRenderer<6> stuff = getBar(parent->assets);
    static SpriteTextRenderer<1> stuff2 = getIcon(parent->assets);

    C2D_DrawRectangle(0, 0, 0, Screen::TOP_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, BLUE_GRADIENT_BOTTOM, BLUE_GRADIENT_BOTTOM, BLUE_GRADIENT_TOP, BLUE_GRADIENT_TOP);
    drawGlossy(0, 0, 0, Screen::TOP_SCREEN_WIDTH, TOP_BAR_HEIGHT);

    for (uint8_t i = 0; i < BARS; i++)
    {
        if (i != 0)
        {
            C2D_DrawRectangle(
                WEEKDAY_WIDTH * i, TOP_BAR_HEIGHT + SPLITTER_MARGIN,
                0,
                2, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT - TOP_BAR_HEIGHT - SPLITTER_MARGIN * 2,
                SPLITTER_START, SPLITTER_END, SPLITTER_START, SPLITTER_END);
        }
        stuff.render(WEEKDAY_WIDTH * (i + 0.5), 120, &tint);
        stuff2.render(WEEKDAY_WIDTH * (i + 0.5), 60, &tint);
    }

    // Bottom buttons
    settingsButton.render(
        Screen::BOTTOM_SCREEN_WIDTH / 2, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT,
        Screen::BOTTOM_SCREEN_WIDTH / 2, BOTTOM_BAR_HEIGHT, parent->assets.staticText.settings);
    C2D_DrawRectangle(
        0, Screen::SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT - 1,
        0,
        Screen::BOTTOM_SCREEN_WIDTH, 2,
        SPLITTER_END, SPLITTER_END, SPLITTER_START, SPLITTER_START);

    C2D_DrawText(&parent->assets.staticText.weeklyWeather, C2D_AlignCenter | C2D_AtBaseline | C2D_WithColor, Screen::BOTTOM_SCREEN_WIDTH / 2, TOP_BAR_HEIGHT * 0.7 + 2, 0, 0.6, 0.6, WHITE);
}