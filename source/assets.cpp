#include "include/assets.h"

void Assets::init()
{
    numbers = C2D_SpriteSheetLoad("romfs:/gfx/numbers.t3x");
    icons = C2D_SpriteSheetLoad("romfs:/gfx/icons.t3x");
}

void Assets::deinit()
{
    C2D_SpriteSheetFree(numbers);
    C2D_SpriteSheetFree(icons);
}