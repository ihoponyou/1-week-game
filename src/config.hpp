#pragma once

#include <string>

const int TILE_SIZE_PX = 32;
namespace GameConstants
{
    const float TIME_SCALE = 1.0f;
    const float PLAYER_ACCEL = 22;
    const float MAX_PLAYER_SPEED = 8;
    const float GRAVITY = 32;
    const float JUMP_POWER = 14;
    const float PLAYER_X_FRICTION = 10;
    const int WORLD_WIDTH = 10;
    const int WORLD_HEIGHT = 10;
    const float COYOTE_TIME = 0.05f;
}

namespace AppConstants
{
    const char* const WINDOW_TITLE = "\"1\" week game";

    const int SCREEN_WIDTH = (GameConstants::WORLD_WIDTH + 2) * TILE_SIZE_PX;
    const int SCREEN_HEIGHT = (GameConstants::WORLD_HEIGHT + 2) * TILE_SIZE_PX;

    inline std::string GET_ASSET_PATH(std::string assetName)
    {
        return ASSETS_PATH "" + assetName;
    }
}
