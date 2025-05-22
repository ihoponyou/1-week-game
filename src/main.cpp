
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
} Player;

bool approximatelyEqual(float a, float b)
{
    return abs(a - b) < FLOAT_DIFF_THRESHOLD;
}

void DrawTile(float x, float y, Color color)
{
    DrawRectangle(x * TILE_SIZE_PX,
                  y * TILE_SIZE_PX,
                  TILE_SIZE_PX,
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
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    Player player = {};
    // player.position = {(float)MAP_WIDTH / 2 - 0.5f,
    //                    (float)MAP_HEIGHT / 2 - 0.5f};

    InitWindow(AppConstants::SCREEN_WIDTH,
               AppConstants::SCREEN_HEIGHT,
               AppConstants::WINDOW_TITLE);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        BeginDrawing();

        ClearBackground(BLACK);

        // ---------------- MAP RENDER ------------------------
        // TODO: pre generate an iterable so we dont check 0s
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (map[y][x] == 0)
                    continue;
                DrawTile(x, y, WHITE);
            }
        }

        // ---------------- PLAYER LOGIC ------------------------
        Vector2 movementInput{};
        if (IsKeyDown(KEY_D))
            movementInput.x += 1;
        if (IsKeyDown(KEY_A))
            movementInput.x -= 1;
        if (IsKeyDown(KEY_W))
            movementInput.y -= 1;
        if (IsKeyDown(KEY_S))
            movementInput.y += 1;

        // acceleration
        float playerSpeed = GameConstants::PLAYER_SPEED;
        player.velocity = Vector2Lerp(player.velocity,
                                      movementInput * playerSpeed,
                                      1 - pow(0.3, deltaTime));
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            player.velocity *= 0.5f;
        }

        player.position += player.velocity * deltaTime;

        // collision
        int playerGridX = static_cast<int>(player.position.x) % MAP_WIDTH;
        int playerGridY = static_cast<int>(player.position.y) % MAP_HEIGHT;

        bool movingRight = player.velocity.x >= 0;
        int tileOffsetX = movingRight ? 1 : 0;
        int topRightTile = map[playerGridY][playerGridX + tileOffsetX];
        int bottomRightTile = map[playerGridY + 1][playerGridX + tileOffsetX];
        if ((approximatelyEqual(player.position.y, playerGridY) &&
             topRightTile) ||
            (approximatelyEqual(player.position.y, playerGridY + 1) &&
             bottomRightTile) ||
            (topRightTile || bottomRightTile))
        {
            player.velocity.x = 0;
            player.position.x = playerGridX + (movingRight ? 0 : 1);
        }

        int tileOffsetY = (player.velocity.y >= 0) ? 1 : -1;

        // ---------------- PLAYER RENDER ------------------------
        // grid position
        DrawText(TextFormat("(%i %i)", playerGridX, playerGridY),
                 100,
                 10,
                 10,
                 GREEN);

        // collision visualizers
        DrawTile(playerGridX + tileOffsetX,
                 playerGridY,
                 topRightTile ? RED : GREEN);
        DrawTile(playerGridX + tileOffsetX,
                 playerGridY + 1,
                 bottomRightTile ? RED : GREEN);

        // cl_showpos
        DrawText(TextFormat("%.2f\n%.2f\n%.2f\n%.2f",
                            player.position.x,
                            player.position.y,
                            player.velocity.x,
                            player.velocity.y),
                 10,
                 10,
                 10,
                 GREEN);

        // player
        DrawTile(player.position.x, player.position.y, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
