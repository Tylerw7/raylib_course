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

    Vector2 ballPosition = { (float)screenWidth/2, (float)screenHeight/2};

    SetTargetFPS(60);

    // Main Loop
    while (!WindowShouldClose())
    {

        // Update
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) ballPosition.x -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) ballPosition.y += 2.0f;
        if (IsKeyDown(KEY_UP)) ballPosition.y -= 2.0f;

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Move the ball with arrow keys", 10, 10, 20, LIGHTGRAY);
            DrawCircleV(ballPosition, 50, MAROON);
        EndDrawing();    
    }

    CloseWindow();

    return 0;
}