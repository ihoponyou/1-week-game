

#include "config.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
} Player;

bool approximatelyEqual(float a, float b)
{
    return abs(a - b) < 0.01f;
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
        {0, 0, 0, 0, 0, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
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

                DrawRectangle(x * TILE_SIZE_PX,
                              y * TILE_SIZE_PX,
                              TILE_SIZE_PX,
                              TILE_SIZE_PX,
                              WHITE);
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
        player.velocity =
            Vector2Lerp(player.velocity,
                        movementInput * GameConstants::PLAYER_SPEED,
                        1 - pow(0.7, deltaTime));

        // collision
        int playerGridX = static_cast<int>(player.position.x) % MAP_WIDTH;
        int playerGridY = static_cast<int>(player.position.y) % MAP_HEIGHT;
        DrawText(TextFormat("(%i %i)", playerGridX, playerGridY),
                 100,
                 10,
                 10,
                 GREEN);
        if (player.velocity.x > 0)
        {
            int topTile = map[playerGridY][playerGridX + 1];
            int bottomTile = map[playerGridY + 1][playerGridX + 1];
            DrawRectangle((playerGridX + 1) * TILE_SIZE_PX,
                          playerGridY * TILE_SIZE_PX,
                          TILE_SIZE_PX,
                          TILE_SIZE_PX,
                          topTile ? MAROON : GREEN);
            DrawRectangle((playerGridX + 1) * TILE_SIZE_PX,
                          (playerGridY + 1) * TILE_SIZE_PX,
                          TILE_SIZE_PX,
                          TILE_SIZE_PX,
                          bottomTile ? MAROON : GREEN);
            if (topTile && bottomTile)
            {
                player.position.x = playerGridX;
                player.velocity.x = 0;
            }
            else if (bottomTile &&
                     !approximatelyEqual(player.position.y, playerGridY))
            {
                player.position.x = playerGridX;
                player.velocity.x = 0;
            }
            else if (topTile &&
                     !approximatelyEqual(player.position.y, playerGridY))
            {

                player.position.x = playerGridX;
                player.velocity.x = 0;
            }
        }

        player.position += player.velocity * deltaTime;

        // ---------------- PLAYER RENDER ------------------------
        DrawText(TextFormat("%.2f\n%.2f\n%.2f\n%.2f",
                            player.position.x,
                            player.position.y,
                            player.velocity.x,
                            player.velocity.y),
                 10,
                 10,
                 10,
                 GREEN);

        Vector2 playerScreenPos = player.position * TILE_SIZE_PX;
        DrawRectangle(playerScreenPos.x,
                      playerScreenPos.y,
                      TILE_SIZE_PX,
                      TILE_SIZE_PX,
                      RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
