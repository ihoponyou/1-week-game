
#include <raylib.h>
#include <raymath.h>

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
} Player;

int main()
{
    int map[10][10] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    };

    Player player = {0};

    InitWindow(800, 600, "GAME");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RED);

        DrawText("YO RAYLIB", 400, 300, 12, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
