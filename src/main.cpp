
#include "config.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

const int LEVEL_WIDTH{10}, LEVEL_HEIGHT{10};

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    bool grounded;
} Player;

void drawScaledRectangle(float posX,
                         float posY,
                         float width,
                         float height,
                         Color color)
{
    DrawRectangle(TILE_SIZE_PX * posX,
                  TILE_SIZE_PX * posY,
                  TILE_SIZE_PX * width,
                  TILE_SIZE_PX * height,
                  color);
}

void drawTile(float x, float y, Color color)
{
    // add 1 since our screen includes out of bounds
    drawScaledRectangle(x + 1, y + 1, 1, 1, color);
}

void drawHorizontalLineAtTile(int x, int y, Color color, float thickness)
{
    drawScaledRectangle(x + 1,
                        y + 1 + (1 - thickness) / 2,
                        1,
                        thickness,
                        color);
}
void drawVerticalLineAtTile(int x, int y, Color color, float thickness)
{
    drawScaledRectangle(x + 1 + (1 - thickness) / 2,
                        y + 1,
                        thickness,
                        1,
                        color);
}

int main()
{
    assert(LEVEL_WIDTH % 2 == 0);
    assert(LEVEL_HEIGHT % 2 == 0);

    int map[LEVEL_WIDTH][LEVEL_HEIGHT] = {
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

    InitWindow(AppConstants::SCREEN_WIDTH,
               AppConstants::SCREEN_HEIGHT,
               AppConstants::WINDOW_TITLE);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        BeginDrawing();

        ClearBackground(BLACK);

        // ---------------- MAP RENDER ------------------------

        for (int y = 0; y < LEVEL_HEIGHT; y++)
        {
            for (int x = 0; x < LEVEL_WIDTH; x++)
            {
                if (map[y][x] == 0)
                {
                    drawTile(x, y, DARKGRAY);
                }
            }
        }

        // ---------------- PLAYER LOGIC ------------------------

        Vector2 movementInput{};
        if (IsKeyDown(KEY_D))
        {
            movementInput.x += 1;
        }
        if (IsKeyDown(KEY_A))
        {
            movementInput.x -= 1;
        }

        // acceleration
        bool movingDown = player.velocity.y >= 0;
        float playerSpeed = GameConstants::PLAYER_SPEED;
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            playerSpeed *= 0.5f;
        }
        if (!movementInput.x)
        {
            player.acceleration.x = player.velocity.x * -8.0f;
        }
        else
        {
            player.acceleration.x = movementInput.x * playerSpeed * 2;
        }

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
        player.velocity += player.acceleration * dt;
        player.velocity = Vector2ClampValue(player.velocity, 0, 100);

        // collision
        player.grounded = false;

        Vector2 newPos{player.position + player.velocity * dt};
        int currentGridX{(int)player.position.x},
            currentGridY{(int)player.position.y};
        int newGridX{(int)newPos.x}, newGridY{(int)newPos.y};
        if (newPos.x + 1 >= LEVEL_WIDTH)
        {
            newPos.x = newGridX;
            player.velocity.x = 0;
        }
        else if (newPos.x < 0)
        {
            newPos.x = 0;
            player.velocity.x = 0;
        }
        else if (map[currentGridY][newGridX] ||
                 map[(int)(player.position.y + 0.9f)][newGridX])
        {
            newPos.x = newGridX + 1;
            player.velocity.x = 0;
        }
        else if (map[currentGridY][newGridX + 1] ||
                 map[(int)(player.position.y + 0.9f)][newGridX + 1])
        {
            newPos.x = newGridX;
            player.velocity.x = 0;
        }

        if (newPos.y + 1 >= LEVEL_HEIGHT)
        {
            newPos.y = newGridY;
            player.velocity.y = 0;
            player.grounded = true;
        }
        else if (newPos.y < 0)
        {
            newPos.y = 0;
            player.velocity.y = 0;
        }
        else if (map[newGridY][currentGridX] ||
                 map[newGridY][(int)(player.position.x + 0.9f)])
        {
            newPos.y = (player.velocity.y >= 0) ? newGridY - 1 : newGridY + 1;
            player.velocity.y = 0;
        }
        else if (map[newGridY + 1][currentGridX] ||
                 map[newGridY + 1][(int)(player.position.x + 0.9f)])
        {
            newPos.y = newGridY;
            player.velocity.y = 0;
            player.grounded = true;
        }

        player.position = newPos;

        // ---------------- PLAYER RENDER ------------------------

        // player
        drawTile(player.position.x, player.position.y, RED);

        // is grounded
        DrawText(player.grounded ? "G" : "", 10, 30, 10, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
