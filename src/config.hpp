#pragma once

const int ScreenScale = 2;
namespace GameConstants
{
    const int WorldWidth = 400;
    const int WorldHeight = 400;
}

namespace AppConstants
{
    const char* const WindowTitle = "one week game";

    const int ScreenWidth = GameConstants::WorldWidth * ScreenScale;
    const int ScreenHeight = GameConstants::WorldHeight * ScreenScale;
}
