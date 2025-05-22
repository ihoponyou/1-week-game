
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
    bool grounded;
} Player;

bool approximatelyEqual(float a, float b)
{
    return abs(a - b) < FLOAT_DIFF_THRESHOLD;
}

void drawTile(float x, float y, Color color)
{
    DrawRectangle((x + 1) * TILE_SIZE_PX,
                  (y + 1) * TILE_SIZE_PX,
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
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    Player player{};
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
        {
            playerSpeed *= 0.1f;
        }
        player.velocity.x = movementInput.x * playerSpeed;
        player.velocity.y += (movingDown ? 18.6f : 9.8f) * deltaTime;
        if (IsKeyPressed(KEY_SPACE) && player.grounded)
        {
            player.velocity.y = -9.8f;
        }
        player.velocity = Vector2ClampValue(player.velocity, 0, 100);
        player.position += player.velocity * deltaTime;

        // collision
        // very reliant on the map being grid-based
        int playerGridX = static_cast<int>(player.position.x);
        int playerGridY = static_cast<int>(player.position.y);

        bool movingRight = player.velocity.x >= 0;
        if ((movingRight && playerGridX >= MAP_WIDTH - 1) ||
            (player.position.x <= 0))
        {
            player.velocity.x = 0;
            player.position.x = playerGridX;
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
        }

        if ((movingDown && playerGridY >= MAP_HEIGHT - 1) ||
            player.position.y <= 0)
        {
            player.velocity.y = 0;
            player.position.y = playerGridY;
            player.grounded = movingDown;
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
        }

        // get move direction
        // get tiles in move direction
        // check if player is axis-aligned
        // if axis aligned and the corresponding tile is obstructing, freeze
        // if not aligned to either axis and there is a tile, freeze

        // out of bounds
        if (player.position.y >= MAP_HEIGHT)
        {
            player.position.y = -1;
        }
        if (player.position.x >= MAP_WIDTH)
        {
            player.position.x = -1;
        }

        // ---------------- PLAYER RENDER ------------------------
        // grid position
        DrawText(TextFormat("(%i %i)", playerGridX, playerGridY),
                 100,
                 10,
                 10,
                 GREEN);

        // collision visualizers
        // DrawTile(playerGridX + tileOffsetX,
        //          playerGridY,
        //          topTile ? MAROON : GREEN);
        // DrawTile(playerGridX + tileOffsetX,
        //          playerGridY + 1,
        //          bottomTile ? MAROON : GREEN);
        // DrawTile(playerGridX,
        //          playerGridY + tileOffsetY,
        //          leftTile ? PINK : YELLOW);
        // DrawTile(playerGridX + 1,
        //          playerGridY + tileOffsetY,
        //          rightTile ? PINK : YELLOW);

        // cl_showpos
        DrawText(TextFormat("%.2f\n%.2f\n%.2f\n%.2f\n%s",
                            player.position.x,
                            player.position.y,
                            player.velocity.x,
                            player.velocity.y,
                            player.grounded ? "G" : ""),
                 10,
                 10,
                 10,
                 GREEN);

        // player
        drawTile(player.position.x, player.position.y, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
