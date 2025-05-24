
#include "helpers.hpp"
#include "config.hpp"
#include <cstdlib>

const float FLOAT_DIFF_THRESHOLD{0.01f};

bool approximatelyEqual(float a, float b)
{
    return abs(a - b) < FLOAT_DIFF_THRESHOLD;
}

void drawTile(float x, float y, Color color)
{
    // add 1 since our screen includes out of bounds
    DrawRectangle((x + 1) * TILE_SIZE_PX,
                  (y + 1) * TILE_SIZE_PX,
                  TILE_SIZE_PX,
                  TILE_SIZE_PX,
                  color);
}

void drawHorizontalLineAtTile(int x, int y, Color color, float thickness)
{
    DrawRectangle((x + 1) * TILE_SIZE_PX,
                  (y + 1 + (1 - thickness) / 2) * TILE_SIZE_PX,
                  TILE_SIZE_PX,
                  thickness * TILE_SIZE_PX,
                  color);
}

void drawVerticalLineAtTile(int x, int y, Color color, float thickness)
{
    DrawRectangle((x + 1 + (1 - thickness) / 2) * TILE_SIZE_PX,
                  (y + 1) * TILE_SIZE_PX,
                  thickness * TILE_SIZE_PX,
                  TILE_SIZE_PX,
                  color);
}
