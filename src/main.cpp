
#include "config.hpp"
#include "helpers.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <format>
#include <fstream>
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

const int LEVEL_WIDTH{10}, LEVEL_HEIGHT{10};

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

    InitWindow(AppConstants::SCREEN_WIDTH,
               AppConstants::SCREEN_HEIGHT,
               AppConstants::WINDOW_TITLE);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime() * GameConstants::TIME_SCALE;

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
