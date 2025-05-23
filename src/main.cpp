
#include "config.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

const int MAP_WIDTH{10};
const int MAP_HEIGHT{10};
const float FLOAT_DIFF_THRESHOLD{0.01f};

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    bool grounded;
} Player;

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

int main()
{
    assert(MAP_WIDTH % 2 == 0);
    assert(MAP_HEIGHT % 2 == 0);

    int map[MAP_WIDTH][MAP_HEIGHT] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    Player player{};

    // SetTargetFPS(60);
    InitWindow(AppConstants::SCREEN_WIDTH,
               AppConstants::SCREEN_HEIGHT,
               AppConstants::WINDOW_TITLE);
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        BeginDrawing();

        ClearBackground(BLACK);

        // ---------------- MAP RENDER ------------------------

        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (map[y][x] == 0)
                    drawTile(x, y, DARKGRAY);
            }
        }

        // ---------------- PLAYER LOGIC ------------------------

        Vector2 movementInput{};
        if (IsKeyDown(KEY_D))
            movementInput.x += 1;
        if (IsKeyDown(KEY_A))
            movementInput.x -= 1;

        // acceleration
        bool movingDown = player.velocity.y >= 0;
        float playerSpeed = GameConstants::PLAYER_SPEED;
        if (IsKeyDown(KEY_LEFT_SHIFT))
            playerSpeed *= 0.5f;
        if (!movementInput.x)
            player.acceleration.x = player.velocity.x * -8.0f;
        else
            player.acceleration.x = movementInput.x * playerSpeed * 2;

        // FIXME: jump height is framerate dependent
        if (player.grounded)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                player.velocity.y = -9.8f;
            }
        }
        else
        {
            player.acceleration.y = (movingDown ? 18.6f : 9.8f);
        }

        // physics
        player.velocity += player.acceleration * deltaTime;
        player.velocity = Vector2ClampValue(player.velocity, 0, 100);
        player.position += player.velocity * deltaTime;

        // collision
        // very reliant on the map being grid-based
        int playerGridX = static_cast<int>(player.position.x);
        int playerGridY = static_cast<int>(player.position.y);

        // FIXME: low framerate causes player to collide with non-existent tiles

        bool movingRight = player.velocity.x >= 0;
        bool xAxisAligned = false;
        bool yAxisAligned = false;
        if ((movingRight && (playerGridX >= (MAP_WIDTH - 1))) ||
            (player.position.x <= 0))
        {
            player.velocity.x = 0;
            player.position.x = playerGridX;
            yAxisAligned = true;
        }
        else
        {
            int tileOffsetX = movingRight ? 1 : 0;
            int topTile = map[playerGridY][playerGridX + tileOffsetX];
            int bottomTile = map[playerGridY + 1][playerGridX + tileOffsetX];

            bool topAxisAligned =
                approximatelyEqual(player.position.y, playerGridY);
            bool bottomAxisAligned =
                approximatelyEqual(player.position.y, playerGridY + 1);

            if ((topAxisAligned && topTile) ||
                (bottomAxisAligned && bottomTile) ||
                ((!topAxisAligned && !bottomAxisAligned) &&
                 (topTile || bottomTile)))
            {
                player.velocity.x = 0;
                player.position.x = playerGridX + (movingRight ? 0 : 1);
            }
            xAxisAligned = topAxisAligned || bottomAxisAligned;
        }

        if ((movingDown && playerGridY >= MAP_HEIGHT - 1) ||
            player.position.y <= 0)
        {
            player.velocity.y = 0;
            player.position.y = playerGridY;
            player.grounded = movingDown;
            xAxisAligned = true;
        }
        else
        {
            int tileOffsetY = movingDown ? 1 : 0;
            int leftTile = map[playerGridY + tileOffsetY][playerGridX];
            int rightTile = map[playerGridY + tileOffsetY][playerGridX + 1];

            bool leftAxisAligned =
                approximatelyEqual(player.position.x, playerGridX);
            bool rightAxisAligned =
                approximatelyEqual(player.position.x, playerGridX + 1);

            if ((leftAxisAligned && leftTile) ||
                (rightAxisAligned && rightTile) ||
                (!(leftAxisAligned || rightAxisAligned) &&
                 (leftTile || rightTile)))
            {
                player.velocity.y = 0;
                player.position.y = playerGridY + (movingDown ? 0 : 1);
                player.grounded = movingDown;
            }
            else
            {
                player.grounded = false;
            }
            yAxisAligned = leftAxisAligned || rightAxisAligned;
        }

        // ---------------- PLAYER RENDER ------------------------

        // player
        drawTile(player.position.x, player.position.y, RED);

        // grid position
        DrawText(TextFormat("(%i, %i)", playerGridX, playerGridY),
                 10,
                 AppConstants::SCREEN_HEIGHT - 20,
                 10,
                 GREEN);

        // cl_showpos
        DrawText(TextFormat("pos_x:%.3f vel_x:%.2f accel_x:%.2f",
                            player.position.x,
                            player.velocity.x,
                            player.acceleration.x),
                 10,
                 10,
                 10,
                 yAxisAligned ? GREEN : DARKGREEN);
        DrawText(TextFormat("pos_y:%.3f vel_y:%.2f accel_y:%.2f",
                            player.position.y,
                            player.velocity.y,
                            player.acceleration.y),
                 10,
                 20,
                 10,
                 xAxisAligned ? GREEN : DARKGREEN);

        // collision visualizers
        drawHorizontalLineAtTile(playerGridX + 1, playerGridY, ORANGE, 0.2f);
        drawHorizontalLineAtTile(playerGridX + 1,
                                 playerGridY + 1,
                                 ORANGE,
                                 0.2f);
        drawVerticalLineAtTile(playerGridX, playerGridY + 1, MAGENTA, 0.2f);
        drawVerticalLineAtTile(playerGridX + 1, playerGridY + 1, MAGENTA, 0.2f);

        // is grounded
        DrawText(player.grounded ? "G" : "", 10, 30, 10, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
