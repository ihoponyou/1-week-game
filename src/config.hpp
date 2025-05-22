#pragma once

const int TILE_SIZE_PX = 64;
namespace GameConstants
{
    const float PLAYER_SPEED = 10;
    const int WORLD_WIDTH = 10;
    const int WORLD_HEIGHT = 10;
}

namespace AppConstants
{
    const char* const WINDOW_TITLE = "1 week game";

    const int SCREEN_WIDTH = GameConstants::WORLD_WIDTH * TILE_SIZE_PX;
    const int SCREEN_HEIGHT = GameConstants::WORLD_HEIGHT * TILE_SIZE_PX;
}
