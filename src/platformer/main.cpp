
#include "raylib.h"



struct Platform 
{
    Rectangle rect;
};



int main()
{
    // Window
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(60);

    //************************
    // WORLD
    //************************

    const float groundY = 520.0f;
    const float worldWidth = WINDOW_WIDTH * 4.0f;

    Platform platforms[] =
    {
        {{0.0f, groundY, worldWidth, 200.0f}},
        {{400.0f, 420.0f, 200.0f, 30.0f}},
        {{750.0f, 340.0f, 200.0f, 30.0f}},
        {{1100.0f, 430.0f, 250.0f, 30.0f}},
        {{1550.0f, 360.0f, 200.0f, 30.0f}},
        {{2000.0f, 290.0f, 300.0f, 30.0f}}
    };

    const int platformCount = sizeof(platforms) / sizeof(platforms[0]);

    //************************
    // PLAYER
    //************************

    Vector2 player = { 100.0f, groundY - 50.0f };

    const float playerSize = 50.0f;
    const float moveSpeed = 300.0f;

    float velocityY = 0.0f;
    float velocityX = 0.0f;
    const float jumpForce = -800.0f;
    const float gravity = 2000.0f;

    bool grounded = true;

    //************************
    // CAMERA
    //************************

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

    //************************
    // GAME LOOP
    //************************

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        //========================
        // INPUT
        //========================

        velocityX = 0.0f;

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            velocityX = moveSpeed;
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            velocityX = -moveSpeed;
        }

        player.x += velocityX * deltaTime;

        // World boundaries
        if (player.x < 0.0f)
        {
            player.x = 0.0f;
        }

        if (player.x + playerSize > worldWidth)
        {
            player.x = worldWidth - playerSize;
        }

        // Jump
        if (IsKeyPressed(KEY_SPACE) && grounded)
        {
            velocityY = jumpForce;
            grounded = false;
        }

        //========================
        // PHYSICS
        //========================

        float previousX = player.x;
        float previousY = player.y;

        // Apply gravity
        velocityY += gravity * deltaTime;

        // Update vertical position
        player.y += velocityY * deltaTime;

        // Assume player is airborne
        grounded = false;

        // Player collision rectangle
        Rectangle playerRect = {
            player.x,
            player.y,
            playerSize,
            playerSize
        };


        for (int i = 0; i < platformCount; i++)
        {
            Rectangle platform = platforms[i].rect;

            if (CheckCollisionRecs(playerRect, platform))
            {
                // Landing on top
                if (velocityY > 0.0f &&
                    previousY + playerSize <= platform.y)
                {
                    player.y = platform.y - playerSize;
                    velocityY = 0.0f;
                    grounded = true;
                }

                // Hitting the bottom
                else if (velocityY < 0.0f &&
                        previousY >= platform.y + platform.height)
                {
                    player.y = platform.y + platform.height;
                    velocityY = 0.0f;
                }

                // Hitting the left side
                if (velocityX > 0.0f &&
                    previousX + playerSize <= platform.x)
                {
                    player.x = platform.x - playerSize;
                }

                // Hitting the right side
                else if (velocityX < 0.0f &&
                        previousX >= platform.x + platform.width)
                {
                    player.x = platform.x + platform.width;
                }
            }

            // Update rectangle after resolving collision
            playerRect.y = player.y;
        }

        //========================
        // CAMERA UPDATE
        //========================

        // Keep camera focused on the player's center
        Vector2 playerCenter = {
            player.x + playerSize / 2.0f,
            player.y + playerSize / 2.0f
        };

        float cameraSmoothing = 5.0f;

        camera.target.x +=
            (playerCenter.x - camera.target.x) *
            cameraSmoothing * deltaTime;

        camera.target.y +=
            (playerCenter.y - camera.target.y) *
            cameraSmoothing * deltaTime;

        // Camera Bounderies
        float halfScreenWidth = WINDOW_WIDTH / (2.0f * camera.zoom);

        float halfScreenHeight = WINDOW_HEIGHT / (2.0f * camera.zoom);

        // Horizontal boundaries
        if (camera.target.x < halfScreenWidth)
        {
            camera.target.x = halfScreenWidth;
        }

        if (camera.target.x > worldWidth - halfScreenWidth)
        {
            camera.target.x = worldWidth - halfScreenWidth;
        }    

        //========================
        // DRAW
        //========================

        BeginDrawing();

        ClearBackground(SKYBLUE);

        // Draw the world using camera coordinates
        BeginMode2D(camera);

        // Ground / Platform
        for (int i = 0; i < platformCount; i++)
        {
            DrawRectangleRec(platforms[i].rect, DARKGREEN);
        }
        

        // Player
        DrawRectangle(
            (int)player.x,
            (int)player.y,
            (int)playerSize,
            (int)playerSize,
            RED
        );

        // Debug collision outlines
        DrawRectangleLinesEx(playerRect, 2.0f, YELLOW);

        EndMode2D();


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
