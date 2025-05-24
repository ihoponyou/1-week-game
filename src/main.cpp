
#include "config.hpp"
#include "helpers.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <format>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>

enum TileType {
    EMPTY,
    SOLID,
    START,
    FINISH,
};

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    bool grounded;
} Player;

const int LEVEL_WIDTH{10};
const int LEVEL_HEIGHT{10};

int main()
{
    SetTraceLogLevel(TraceLogLevel::LOG_WARNING);

    assert(LEVEL_WIDTH % 2 == 0);
    assert(LEVEL_HEIGHT % 2 == 0);

    // ---------------- LEVEL LOADING -------------------

    int levelIndex{1};
    std::ifstream levelFile{std::format("../levels/{}.txt", levelIndex)};
    assert(levelFile.is_open() && "failed to load level");

    TileType levelTiles[LEVEL_HEIGHT][LEVEL_WIDTH];
    int row{};
    std::string line;
    while (std::getline(levelFile, line))
    {
        int columns{std::min(LEVEL_WIDTH, static_cast<int>(line.length()))};
        for (int column{}; column < columns; column++)
        {
            TileType tileType;
            switch (line.at(column))
            {
            case '1':
                tileType = TileType::SOLID;
                break;
            case 's':
                tileType = TileType::START;
                break;
            case 'f':
                tileType = TileType::FINISH;
                break;
            default:
                tileType = TileType::EMPTY;
            }
            levelTiles[row][column] = tileType;
        }

        row++;
        if (row >= LEVEL_HEIGHT)
        {
            break;
        }
    }
    levelFile.close();

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

        // ---------------- level RENDER ------------------------

        for (int y = 0; y < LEVEL_HEIGHT; y++)
        {
            for (int x = 0; x < LEVEL_WIDTH; x++)
            {
                Color tileColor{PURPLE};
                switch (levelTiles[y][x])
                {
                case TileType::EMPTY:
                    tileColor = DARKGRAY;
                    break;
                case TileType::SOLID:
                    tileColor = GRAY;
                    break;
                case TileType::START:
                    tileColor = MAROON;
                    break;
                case TileType::FINISH:
                    tileColor = DARKBLUE;
                    break;
                }
                drawTile(x, y, tileColor);
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
        // very reliant on the level being grid-based
        int playerGridX = static_cast<int>(player.position.x);
        int playerGridY = static_cast<int>(player.position.y);

        // FIXME: low framerate causes player to collide with non-existent
        // tiles

        bool movingRight = player.velocity.x >= 0;
        bool xAxisAligned = false;
        bool yAxisAligned = false;
        if ((movingRight && (playerGridX >= (LEVEL_WIDTH - 1))) ||
            (player.position.x <= 0))
        {
            player.velocity.x = 0;
            player.position.x = playerGridX;
            yAxisAligned = true;
        }
        else
        {
            int tileOffsetX = movingRight ? 1 : 0;
            int topTile = levelTiles[playerGridY][playerGridX + tileOffsetX];
            int bottomTile =
                levelTiles[playerGridY + 1][playerGridX + tileOffsetX];

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

        if ((movingDown && playerGridY >= LEVEL_HEIGHT - 1) ||
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
            int leftTile = levelTiles[playerGridY + tileOffsetY][playerGridX];
            int rightTile =
                levelTiles[playerGridY + tileOffsetY][playerGridX + 1];

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
