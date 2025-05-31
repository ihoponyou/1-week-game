
#include "config.hpp"
#include <algorithm>
#include <cassert>
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
    bool hasJumped;
    float airTime;
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

// not sure what convention is for functions that modify parameters
bool isCollidableTile(TileType map[][LEVEL_HEIGHT],
                      float x,
                      float y,
                      TileType& tileType)
{
    tileType = map[static_cast<int>(y)][static_cast<int>(x)];
    return tileType == TileType::SOLID || tileType == TileType::FINISH;
}

void loadLevel(TileType level[LEVEL_WIDTH][LEVEL_HEIGHT],
               Player& player,
               const std::string& levelName)
{
    std::string fileName = std::format("../levels/{}.txt", levelName);
    std::cout << fileName << "\n";
    std::ifstream levelFile{fileName};
    assert(levelFile.is_open());

    int startX{}, startY{};
    int x{};
    std::string line;
    while (std::getline(levelFile, line))
    {
        int columns{std::min(LEVEL_WIDTH, static_cast<int>(line.length()))};
        for (int y{}; y < columns; y++)
        {
            TileType tileType;
            switch (line.at(y))
            {
            case '1':
                tileType = TileType::SOLID;
                break;
            case 's':
                startX = y;
                startY = x;
                tileType = TileType::START;
                break;
            case 'f':
                tileType = TileType::FINISH;
                break;
            default:
                tileType = TileType::EMPTY;
            }
            level[x][y] = tileType;
        }

        x++;
        if (x >= LEVEL_HEIGHT)
        {
            break;
        }
    }

    player.position.x = startX;
    player.position.y = startY;
    player.velocity = {};

    levelFile.close();
}
void loadLevel(TileType level[LEVEL_WIDTH][LEVEL_HEIGHT],
               Player& player,
               int levelIndex)
{
    loadLevel(level, player, std::to_string(levelIndex));
}

int main(int argc, char* argv[])
{
    SetTraceLogLevel(TraceLogLevel::LOG_WARNING);

    TileType levelTiles[LEVEL_HEIGHT][LEVEL_WIDTH];
    int levelIndex{6};
    Player player{};
    player.hasJumped = false;

    loadLevel(levelTiles, player, levelIndex);

    InitWindow(AppConstants::SCREEN_WIDTH,
               AppConstants::SCREEN_HEIGHT,
               AppConstants::WINDOW_TITLE);

    Image icon = LoadImage("../resources/icon.png");
    SetWindowIcon(icon);
    Texture2D texture = LoadTextureFromImage(icon);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime() * GameConstants::TIME_SCALE;

        BeginDrawing();

        ClearBackground(BLACK);

        if (levelIndex >= 6)
        {
            DrawTexture(texture, 0, 0, WHITE);
            EndDrawing();
            continue;
        }

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
                    tileColor = DARKBLUE;
                    break;
                case TileType::FINISH:
                    tileColor = BLUE;
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
        float playerSpeed = GameConstants::PLAYER_ACCEL;
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            playerSpeed *= 0.5f;
        }
        if (!movementInput.x)
        {
            player.acceleration.x =
                player.velocity.x * -GameConstants::PLAYER_X_FRICTION;
        }
        else
        {
            player.acceleration.x = movementInput.x * playerSpeed;
        }

        // FIXME: jump height is framerate dependent
        player.acceleration.y = GameConstants::GRAVITY;
        if (IsKeyPressed(KEY_SPACE) && !player.hasJumped &&
            player.airTime < GameConstants::COYOTE_TIME)
        {
            player.hasJumped = true;
            player.velocity.y = -GameConstants::JUMP_POWER;
        }

        // physics
        player.velocity += player.acceleration * dt;
        player.velocity.x = std::clamp(player.velocity.x,
                                       -GameConstants::MAX_PLAYER_SPEED,
                                       GameConstants::MAX_PLAYER_SPEED);

        // collision
        player.grounded = false;
        player.airTime += dt;

        TileType collidedTileType{};
        Vector2 newPos{player.position + player.velocity * dt};
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
        else if (isCollidableTile(levelTiles,
                                  newPos.x,
                                  player.position.y,
                                  collidedTileType) ||
                 isCollidableTile(levelTiles,
                                  newPos.x,
                                  player.position.y + 0.9f,
                                  collidedTileType))
        {
            if (collidedTileType == TileType::FINISH)
            {
                loadLevel(levelTiles, player, ++levelIndex);
                continue;
            }

            newPos.x = newGridX + 1;
            player.velocity.x = 0;
        }
        else if (isCollidableTile(levelTiles,
                                  newGridX + 1,
                                  player.position.y,
                                  collidedTileType) ||
                 isCollidableTile(levelTiles,
                                  newGridX + 1,
                                  player.position.y + 0.9f,
                                  collidedTileType))
        {
            if (collidedTileType == TileType::FINISH)
            {
                loadLevel(levelTiles, player, ++levelIndex);
                continue;
            }

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
        else if (isCollidableTile(levelTiles,
                                  player.position.x,
                                  newGridY,
                                  collidedTileType) ||
                 isCollidableTile(levelTiles,
                                  player.position.x + 0.9f,
                                  newGridY,
                                  collidedTileType))
        {
            if (collidedTileType == TileType::FINISH)
            {
                loadLevel(levelTiles, player, ++levelIndex);
                continue;
            }

            newPos.y = (player.velocity.y >= 0) ? newGridY - 1 : newGridY + 1;
            player.velocity.y = 0;
        }
        else if (isCollidableTile(levelTiles,
                                  player.position.x,
                                  newGridY + 1,
                                  collidedTileType) ||
                 isCollidableTile(levelTiles,
                                  player.position.x + 0.9f,
                                  newGridY + 1,
                                  collidedTileType))
        {
            if (collidedTileType == TileType::FINISH)
            {
                loadLevel(levelTiles, player, ++levelIndex);
                continue;
            }

            if (player.velocity.y > 0)
            {
                newPos.y = newGridY;
                player.velocity.y = 0;
                player.grounded = true;
            }
        }

        if (player.grounded)
        {
            player.airTime = 0;
            player.hasJumped = false;
        }

        player.position = newPos;

        // ---------------- PLAYER RENDER ------------------------

        // player
        drawTile(player.position.x, player.position.y, RED);

        // is grounded
        DrawText(player.grounded ? "G" : "", 10, 30, 10, GREEN);

        EndDrawing();
    }

    UnloadTexture(texture);

    CloseWindow();

    return 0;
}
