
#include "raylib.h"




int main()
{
    // Window
    int WINDOW_HEIGHT = 1280;
    int WINDOW_WIDTH = 720;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "platformer");
    SetTargetFPS(60);

    //******************* 
    // WORLD
    //******************* 

    const float groundY = 520.0f;
    const float groundWidth = WINDOW_WIDTH * 4.0f;

    // PLAYER

    const float playerSize = 50.0f;

    Vector2 player = {
        groundWidth / 2.0f,
        groundY - playerSize,
    };


    float velocityY = 0.0f;

    const float moveSpeed = 300.0f;
    const float jumpForce = -600.0f;
    const float gravity = 1500.0f;

    bool grounded = true;

    // -------------------------
    // CAMERA
    // -------------------------

    Camera2D camera = { 0 };

    camera.target = {
        player.x + playerSize / 2.0f,
        player.y + playerSize / 2.0f
    };

    camera.offset = {
        WINDOW_WIDTH / 2.0f,
        WINDOW_HEIGHT / 2.0f
    };

    camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    // -------------------------
    // GAME LOOP
    // -------------------------

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // -------------------------
        // PLAYER MOVEMENT
        // -------------------------

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            player.x -= moveSpeed * deltaTime;
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            player.x += moveSpeed * deltaTime;
        }

        // Keep player inside the ground
        if (player.x < 0)
            player.x = 0;

        if (player.x > groundWidth - playerSize)
            player.x = groundWidth - playerSize;

        // -------------------------
        // JUMPING
        // -------------------------

        if (IsKeyPressed(KEY_SPACE) && grounded)
        {
            velocityY = jumpForce;
            grounded = false;
        }

        // Gravity
        velocityY += gravity * deltaTime;

        player.y += velocityY * deltaTime;

        // Ground collision
        if (player.y + playerSize >= groundY)
        {
            player.y = groundY - playerSize;
            velocityY = 0;
            grounded = true;
        }

        // -------------------------
        // CAMERA FOLLOW
        // -------------------------

        camera.target = {
            player.x + playerSize / 2.0f,
            player.y + playerSize / 2.0f
        };

        // -------------------------
        // DRAWING
        // -------------------------

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode2D(camera);

            // Ground
            DrawRectangle(
                0,
                groundY,
                groundWidth,
                200,
                DARKGREEN
            );

            // Player
            DrawRectangle(
                (int)player.x,
                (int)player.y,
                (int)playerSize,
                (int)playerSize,
                RED
            );

        EndMode2D();

        DrawText("A/D or Arrow Keys: Move", 20, 20, 20, BLACK);
        DrawText("SPACE: Jump", 20, 50, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();



    return 0;
}