
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
    bool facingLeft;

    int currentFrame;
    float animationTimer;

    Texture2D texture;
};

struct Platform
{
    Rectangle rect;
};

struct Level
{
    Platform* Platforms;
    int platformCount;
    float worldWidth;
    Vector2 playerSpawn;
};


//**************************************
// Level Data
//**************************************
const float GROUND_Y = 520.0f;

Platform level1Platforms[] =
{
    {{0, GROUND_Y, 5120, 200}},
    {{400, 420, 200, 30}},
    {{750, 340, 200, 30}},
    {{1100, 430, 250, 30}},
    {{1550, 360, 200, 30}},
    {{2000, 290, 300, 30}}
};

Platform level2Platforms[] =
{
    {{0, GROUND_Y, 5120, 200}},
    {{300, 400, 200, 30}},
    {{650, 320, 250, 30}},
    {{1050, 250, 200, 30}},
    {{1450, 380, 300, 30}},
    {{1950, 300, 250, 30}}
};



Level levels[] =
{
    {
        level1Platforms,
        sizeof(level1Platforms) / sizeof(level1Platforms[0]),
        5120.0f,
        {100.0f, GROUND_Y - 50.0f}
    },

    {
        level2Platforms,
        sizeof(level2Platforms) / sizeof(level2Platforms[0]),
        5120.0f,
        {100.0f, GROUND_Y - 50.0f}
    }
};

const int levelCount = sizeof(levels) / sizeof(levels[0]);





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

void LoadLevel(
    Player& player,
    Camera2D& camera,
    const Level& level
);






//**************************************
// MAIN
//**************************************

int main()
{
    // Window
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    // LEVEL INFO
    int currentLevelIndex = 0;
    Level& currentLevel = levels[currentLevelIndex];

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(60);

    // Resolve asset paths relative to the executable, not the launch directory,
    // so the game works no matter where it's run from.
    ChangeDirectory(GetApplicationDirectory());

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
    player.facingLeft = false;

    player.currentFrame = 0;
    player.animationTimer = 0.0f;

    player.texture = LoadTexture("assets/player_run_animation.png");

    if (player.texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load player texture!");
        CloseWindow();
        return 1;
    }

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

        if (IsKeyPressed(KEY_ONE))
        {
            currentLevelIndex = 0;

            LoadLevel(player, camera, levels[currentLevelIndex]);
        }

        if (IsKeyPressed(KEY_TWO))
        {
            currentLevelIndex = 1;

            LoadLevel(player, camera, levels[currentLevelIndex]);
        }

        Level& currentLevel = levels[currentLevelIndex];

       // Save previous positions
       float previousX = player.position.x;
       float previousY = player.position.y;

       // Update
       UpdatePlayer(player, deltaTime);

       // Collisions
       ResolveCollisions(player,currentLevel.Platforms, currentLevel.platformCount,previousX,previousY);

           // Camera
       UpdateCamera(
            camera,
            player,
            deltaTime,
            currentLevel.worldWidth,
            WINDOW_WIDTH
        );

        // Renderer
        BeginDrawing();

        ClearBackground(SKYBLUE);
        BeginMode2D(camera);

        DrawWorld(currentLevel.Platforms, currentLevel.platformCount);
        DrawPlayer(player);

        EndMode2D();

        EndDrawing();


    }

    UnloadTexture(player.texture);

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
        player.facingLeft = false;
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        player.velocity.x = -player.moveSpeed;
        player.facingLeft = true;
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

    // Animation
    const float frameDuration = 0.10f;

    bool moving =player.velocity.x != 0.0f;

    if (moving)
    {
        player.animationTimer += deltaTime;

        while(player.animationTimer >= frameDuration)
        {
            player.animationTimer -= frameDuration;

            player.currentFrame++;

            int frameCount = 5;

            if (player.facingLeft)
            {
                frameCount = 4;
            }

            player.currentFrame %= frameCount;
        }
    }
    else
    {
        player.currentFrame = 0;
        player.animationTimer = 0.0f;
    }
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
    const int FRAME_WIDTH = 144;
    const int FRAME_HEIGHT = 192;

    int animationRow = player.facingLeft ? 1 : 0;

    Rectangle source = {
        (float)(player.currentFrame * FRAME_WIDTH),
        (float)(animationRow * FRAME_HEIGHT),
        (float)FRAME_WIDTH,
        (float)FRAME_HEIGHT
    };

    float drawHeight = player.size * 1.3333f;

    Rectangle destination = {
        player.position.x,
        player.position.y + player.size - drawHeight,
        player.size,
        drawHeight
    };

    Vector2 origin = {0.0f, 0.0f};

    DrawTexturePro(
        player.texture,
        source,
        destination,
        origin,
        0.0f,
        WHITE
    );
}

void DrawWorld(Platform platforms[], int platformCount)
{
    for (int i = 0; i < platformCount; i++)
    {
        DrawRectangleRec(platforms[i].rect, DARKGREEN);

    }
}


void LoadLevel(
    Player& player,
    Camera2D& camera,
    const Level& level
)
{
    // Reset player position
    player.position = level.playerSpawn;

    // Reset velocity
    player.velocity = {0.0f, 0.0f};

    // Reset grounded state
    player.grounded = true;

    // Reset camera
    camera.target = {
        player.position.x + player.size / 2.0f,
        player.position.y + player.size / 2.0f
    };
}