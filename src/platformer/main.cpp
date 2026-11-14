
#include "raylib.h"







//**************************************
// STRUCTURES
//**************************************

struct Player
{
    Vector2 position;
    Vector2 velocity;

    float size;
    float moveSpeed;
    float jumpForce;
    float gravity;

    bool grounded;
};

struct Platform
{
    Rectangle rect;
};





//**************************************
// Function Declarations
//**************************************

void UpdatePlayer(Player& player, float deltaTime);

void ResolveCollisions(
    Player& player,
    Platform platforms[],
    int platformCount,
    float previousX,
    float previousY
);

void UpdateCamera(
    Camera2D& camera,
    const Player& player,
    float deltaTime,
    float worldWidth,
    int windowWidth
);

void DrawPlayer(const Player& player);

void DrawWorld(Platform platforms[], int platformCount);






//**************************************
// MAIN
//**************************************

int main()
{
    // Window
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(60);

    //**************************************
    // WORLD
    //**************************************

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

    const int platformCount =
        sizeof(platforms) / sizeof(platforms[0]);

    //**************************************
    // PLAYER
    //**************************************

    Player player = {};

    player.position = {100.0f, groundY - 50.0f};
    player.velocity = {0.0f, 0.0f};

    player.size = 50.0f;
    player.moveSpeed = 300.0f;
    player.jumpForce = -800.0f;
    player.gravity = 2000.0f;

    player.grounded = true;

    //**************************************
    // CAMERA
    //**************************************

    Camera2D camera = {0};

    camera.target = {
        player.position.x + player.size / 2.0f,
        player.position.y + player.size / 2.0f
    };

    camera.offset = {
        WINDOW_WIDTH / 2.0f,
        WINDOW_HEIGHT / 2.0f
    };

    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    const float cameraSmoothing = 5.0f;

    //**************************************
    // GAME LOOP
    //**************************************

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

       // Save previous positions
       float previousX = player.position.x;
       float previousY = player.position.y;

       // Update
       UpdatePlayer(player, deltaTime);

       // Collisions
       ResolveCollisions(player,platforms, platformCount,previousX,previousY);

           // Camera
       UpdateCamera(
            camera,
            player,
            deltaTime,
            worldWidth,
            WINDOW_WIDTH
        );

        // Renderer
        BeginDrawing();

        ClearBackground(SKYBLUE);
        BeginMode2D(camera);

        DrawWorld(platforms, platformCount);
        DrawPlayer(player);

        EndMode2D();

        EndDrawing();


    }

    CloseWindow();

    return 0;
}



void UpdatePlayer(Player& player, float deltaTime)
{
    // Horizontal movement
    player.velocity.x = 0.0f;

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        player.velocity.x = player.moveSpeed;
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        player.velocity.x = -player.moveSpeed;
    }

    // Jump
    if (IsKeyPressed(KEY_SPACE) && player.grounded)
    {
        player.velocity.y = player.jumpForce;
        player.grounded = false;
    }

    // Gravity
    player.velocity.y += player.gravity * deltaTime;

    // Position
    player.position.x += player.velocity.x * deltaTime;
    player.position.y += player.velocity.y * deltaTime;
}


void ResolveCollisions(
    Player& player,
    Platform platforms[],
    int platformCount,
    float previousX,
    float previousY
)
{
    player.grounded = false;

    Rectangle playerRect = {
        player.position.x,
        player.position.y,
        player.size,
        player.size
    };

    for (int i = 0; i < platformCount; i++)
    {
        Rectangle platform = platforms[i].rect;

        if (!CheckCollisionRecs(playerRect, platform))
        {
            continue;
        }

        // Landing on top
        if (player.velocity.y > 0.0f &&
            previousY + player.size <= platform.y)
        {
            player.position.y = platform.y - player.size;
            player.velocity.y = 0.0f;
            player.grounded = true;
        }
        // Hitting bottom
        else if (player.velocity.y < 0.0f &&
                 previousY >= platform.y + platform.height)
        {
            player.position.y = platform.y + platform.height;
            player.velocity.y = 0.0f;
        }
        // Hitting left side
        else if (player.velocity.x > 0.0f &&
                 previousX + player.size <= platform.x)
        {
            player.position.x = platform.x - player.size;
        }
        // Hitting right side
        else if (player.velocity.x < 0.0f &&
                 previousX >= platform.x + platform.width)
        {
            player.position.x = platform.x + platform.width;
        }

        playerRect.x = player.position.x;
        playerRect.y = player.position.y;
    }
}


void UpdateCamera(
    Camera2D& camera,
    const Player& player,
    float deltaTime,
    float worldWidth,
    int windowWidth
)
{
    Vector2 playerCenter = {
        player.position.x + player.size / 2.0f,
        player.position.y + player.size / 2.0f
    };

    float smoothing = 5.0f;

    camera.target.x +=
        (playerCenter.x - camera.target.x) *
        smoothing * deltaTime;

    camera.target.y +=
        (playerCenter.y - camera.target.y) *
        smoothing * deltaTime;

    float halfScreenWidth =
        windowWidth / (2.0f * camera.zoom);

    if (camera.target.x < halfScreenWidth)
        camera.target.x = halfScreenWidth;

    if (camera.target.x > worldWidth - halfScreenWidth)
        camera.target.x = worldWidth - halfScreenWidth;
}


void DrawPlayer(const Player& player)
{
    DrawRectangle(
        (int)player.position.x,
        (int)player.position.y,
        (int)player.size,
        (int)player.size,
        RED
    );
}

void DrawWorld(Platform platforms[], int platformCount)
{
    for (int i = 0; i < platformCount; i++)
    {
        DrawRectangleRec(platforms[i].rect, DARKGREEN);

    }
}
