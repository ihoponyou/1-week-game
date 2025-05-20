
#include "config.hpp"
#include <cassert>
#include <cstdio>
#include <raylib.h>
#include <raymath.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define TILE_SIZE_PX 16

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
} Player;

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
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    Player player = {};

    Vector2 windowCenter = {(float)AppConstants::ScreenWidth / 2,
                            (float)AppConstants::ScreenHeight / 2};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(AppConstants::ScreenWidth,
               AppConstants::ScreenHeight,
               AppConstants::WindowTitle);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        BeginDrawing();

        ClearBackground(BLACK);
        DrawText(TextFormat("%f", deltaTime), 10, 40, 20, YELLOW);

        // map render
        // TODO: pre generate an iterable so we dont check 0s
        int mapCenterIndexX{MAP_WIDTH / 2};
        int mapCenterIndexY{MAP_HEIGHT / 2};
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (map[y][x] == 0)
                    continue;

                int tileOffsetX = x - mapCenterIndexX;
                int tileOffsetY = y - mapCenterIndexY;
                int posX = windowCenter.x + tileOffsetX * TILE_SIZE_PX;
                int posY = windowCenter.y + tileOffsetY * TILE_SIZE_PX;

                DrawRectangle(posX, posY, TILE_SIZE_PX, TILE_SIZE_PX, WHITE);
            }
        }

        // player logic
        if (IsKeyDown(KEY_D))
            player.velocity.x += 1;
        if (IsKeyDown(KEY_A))
            player.velocity.x -= 1;
        if (IsKeyDown(KEY_W))
            player.velocity.y -= 1;
        if (IsKeyDown(KEY_S))
            player.velocity.y += 1;

        player.velocity.x = Clamp(player.velocity.x, -10, 10);
        player.velocity.y = Clamp(player.velocity.y, -10, 10);
        player.position += player.velocity * deltaTime;

        // player render
        DrawText(TextFormat("%f\n%f\n%f\n%f",
                            player.position.x,
                            player.position.y,
                            player.velocity.x,
                            player.velocity.y),
                 100,
                 10,
                 20,
                 GREEN);
        DrawRectangle(player.position.x - ((float)TILE_SIZE_PX / 2),
                      player.position.y - TILE_SIZE_PX,
                      TILE_SIZE_PX,
                      TILE_SIZE_PX,
                      RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
