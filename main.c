#include "raylib.h"

int main(void)
{
    InitWindow(1200, 700, "Montanha-Russa 2D");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Simulador de Montanha-Russa 2D",
                 350, 100, 30, BLACK);

        DrawCircle(600, 350, 50, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}