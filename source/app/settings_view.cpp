#include "settings_view.h"
#include "app.h"

constexpr uint32_t RED = C2D_Color32(255, 0, 0, 255);

SettingsView::SettingsView(App *app) : parent(app)
{
}

void SettingsView::poll()
{
}

void SettingsView::renderTop()
{
    C2D_DrawRectSolid(0, 0, 0, Screen::TOP_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, RED);
}

void SettingsView::renderBottom()
{
    C2D_DrawRectSolid(0, 0, 0, Screen::BOTTOM_SCREEN_WIDTH, Screen::SCREEN_HEIGHT, RED);
}