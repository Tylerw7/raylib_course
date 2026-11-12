
#include "raylib.h"




int main()
{
    //Window
    int windowWidth = 1280;
    int windowHeight = 720;

    InitWindow(windowWidth, windowHeight, "platformer");
    SetTargetFPS(60);

    struct Vector2
    {
        float x;
        float y;
    };

    Vector2 player = {100.0f, 470.f};

    const float playerSize = 50.0f;
    const float moveSpeed = 300.0f;
    float velocityY = 0.0f;

    const float jumpForce = -600.0f;
    const float gravity = 2000.0f;
    bool grounded = true;
    const float groundY = 520.0f;



    // DrawRectangle(posX, posY, width, height, color);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            player.x += moveSpeed * deltaTime;
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            player.x -= moveSpeed * deltaTime;
        }

        if (IsKeyPressed(KEY_SPACE) && grounded)
        {
            velocityY = jumpForce;
            grounded = false;
        }

        // Gravity
        velocityY += gravity * deltaTime;
        player.y += velocityY * deltaTime;

        // Ground
        if (player.y + playerSize >= groundY)
        {
            velocityY = 0.0f;
            grounded = true;
            player.y = groundY - playerSize;
        }

        BeginDrawing();

        ClearBackground(SKYBLUE);
        // Ground
        DrawRectangle(0, 520, 1280, 200, DARKGREEN);

        // Player
        DrawRectangle(
            (int)player.x,
            (int)player.y,
            (int)playerSize,
            (int)playerSize, 
            RED
        );


        EndDrawing();
    }

    CloseWindow();



    return 0;
}