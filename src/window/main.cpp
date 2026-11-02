#include <iostream>
#include "raylib.h"
#include <vector>


int main()
{

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "basic window");

    SetTargetFPS(60);

    // Main Loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window", 190, 200, 20, LIGHTGRAY);
        EndDrawing();    
    }

    return 0;
}