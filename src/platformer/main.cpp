
#include "raylib.h"

// ASSET_DIR is defined by CMake as an absolute path to this example's assets folder.
// Fall back to a relative path only if building without CMake.
#ifndef ASSET_DIR
#define ASSET_DIR "assets"
#endif

//****************************************
// ANIMATION STATES
//****************************************

enum class AnimationState
{
    IDLE,
    RUNNING,
    JUMPING,
    FALLING
};


//****************************************
// STRUCTURES
//****************************************

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

    Texture2D spriteSheet;

    AnimationState animationState;

    int currentFrame;
    float animationTimer;
};

struct Platform
{
    Rectangle rect;
};

struct Level
{
    Platform* platforms;
    int platformCount;

    float worldWidth;

    Vector2 playerSpawn;
};


//****************************************
// LEVEL DATA
//****************************************

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


//****************************************
// FUNCTION DECLARATIONS
//****************************************

void UpdatePlayer(Player& player, float deltaTime);

void ResolveCollisions(
    Player& player,
    Platform platforms[],
    int platformCount,
    float previousX,
    float previousY
);

void UpdateAnimation(Player& player, float deltaTime);

void UpdateCamera(
    Camera2D& camera,
    const Player& player,
    float deltaTime,
    float worldWidth,
    int windowWidth
);

void DrawPlayer(const Player& player);

void DrawWorld(Platform platforms[], int platformCount);

void DrawHUD(int currentLevelIndex);

void LoadLevel(
    Player& player,
    Camera2D& camera,
    const Level& level
);


//****************************************
// MAIN
//****************************************

int main()
{
    // WINDOW
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    InitWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Raylib Platformer"
    );

    SetTargetFPS(60);


    //****************************************
    // PLAYER
    //****************************************

    Player player = {};

    player.position = {100.0f, GROUND_Y - 50.0f};
    player.velocity = {0.0f, 0.0f};

    player.size = 50.0f;

    player.moveSpeed = 300.0f;
    player.jumpForce = -800.0f;
    player.gravity = 2000.0f;

    player.grounded = false;
    player.facingLeft = false;

    player.animationState = AnimationState::IDLE;

    player.currentFrame = 0;
    player.animationTimer = 0.0f;


    //****************************************
    // LOAD SPRITE SHEET
    //****************************************

    player.spriteSheet =
        LoadTexture(ASSET_DIR "/player_run_animation.png");

    if (player.spriteSheet.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load player sprite sheet!");

        CloseWindow();
        return 1;
    }

    // Our sprite sheet has 5 columns and 2 rows.
    // Calculate dimensions from the actual loaded image.

    const float FRAME_WIDTH =
        player.spriteSheet.width / 5.0f;

    const float FRAME_HEIGHT =
        player.spriteSheet.height / 2.0f;


    //****************************************
    // CAMERA
    //****************************************

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


    //****************************************
    // LEVEL MANAGEMENT
    //****************************************

    int currentLevelIndex = 0;

    LoadLevel(
        player,
        camera,
        levels[currentLevelIndex]
    );


    //****************************************
    // GAME LOOP
    //****************************************

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();


        //****************************************
        // LEVEL SWITCHING
        //****************************************

        if (IsKeyPressed(KEY_ONE))
        {
            currentLevelIndex = 0;

            LoadLevel(
                player,
                camera,
                levels[currentLevelIndex]
            );
        }

        if (IsKeyPressed(KEY_TWO))
        {
            currentLevelIndex = 1;

            LoadLevel(
                player,
                camera,
                levels[currentLevelIndex]
            );
        }


        // Get current level
        Level& currentLevel = levels[currentLevelIndex];


        //****************************************
        // SAVE PREVIOUS POSITION
        //****************************************

        float previousX = player.position.x;
        float previousY = player.position.y;


        //****************************************
        // UPDATE PLAYER
        //****************************************

        UpdatePlayer(player, deltaTime);


        //****************************************
        // COLLISIONS
        //****************************************

        ResolveCollisions(
            player,
            currentLevel.platforms,
            currentLevel.platformCount,
            previousX,
            previousY
        );


        //****************************************
        // ANIMATION
        //****************************************

        UpdateAnimation(player, deltaTime);


        //****************************************
        // CAMERA
        //****************************************

        UpdateCamera(
            camera,
            player,
            deltaTime,
            currentLevel.worldWidth,
            WINDOW_WIDTH
        );


        //****************************************
        // DRAW
        //****************************************

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode2D(camera);

        DrawWorld(
            currentLevel.platforms,
            currentLevel.platformCount
        );

        DrawPlayer(player);

        EndMode2D();

        DrawHUD(currentLevelIndex);

        EndDrawing();
    }


    //****************************************
    // CLEANUP
    //****************************************

    UnloadTexture(player.spriteSheet);

    CloseWindow();

    return 0;
}


//****************************************
// UPDATE PLAYER
//****************************************

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


    // Jumping

    if (IsKeyPressed(KEY_SPACE) && player.grounded)
    {
        player.velocity.y = player.jumpForce;

        player.grounded = false;
    }


    // Gravity

    player.velocity.y += player.gravity * deltaTime;


    // Move player

    player.position.x += player.velocity.x * deltaTime;
    player.position.y += player.velocity.y * deltaTime;
}


//****************************************
// COLLISION RESOLUTION
//****************************************

void ResolveCollisions(
    Player& player,
    Platform platforms[],
    int platformCount,
    float previousX,
    float previousY
)
{
    player.grounded = false;

    Rectangle playerRect =
    {
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


        // Landing on platform

        if (player.velocity.y > 0.0f &&
            previousY + player.size <= platform.y)
        {
            player.position.y = platform.y - player.size;

            player.velocity.y = 0.0f;

            player.grounded = true;
        }

        // Hitting underside

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

            player.velocity.x = 0.0f;
        }

        // Hitting right side

        else if (player.velocity.x < 0.0f &&
                 previousX >= platform.x + platform.width)
        {
            player.position.x = platform.x + platform.width;

            player.velocity.x = 0.0f;
        }

        // Update collision rectangle

        playerRect.x = player.position.x;
        playerRect.y = player.position.y;
    }
}


//****************************************
// UPDATE ANIMATION
//****************************************

void UpdateAnimation(Player& player, float deltaTime)
{
    // Determine current animation state

    if (!player.grounded)
    {
        if (player.velocity.y < 0.0f)
        {
            player.animationState = AnimationState::JUMPING;
        }
        else
        {
            player.animationState = AnimationState::FALLING;
        }
    }
    else if (player.velocity.x != 0.0f)
    {
        player.animationState = AnimationState::RUNNING;
    }
    else
    {
        player.animationState = AnimationState::IDLE;
    }


    // Only animate running for now

    if (player.animationState == AnimationState::RUNNING)
    {
        const float frameDuration = 0.10f;

        player.animationTimer += deltaTime;

        while (player.animationTimer >= frameDuration)
        {
            player.animationTimer -= frameDuration;

            player.currentFrame++;

            // Four running frames

            player.currentFrame %= 4;
        }
    }
    else
    {
        player.currentFrame = 0;

        player.animationTimer = 0.0f;
    }
}


//****************************************
// UPDATE CAMERA
//****************************************

void UpdateCamera(
    Camera2D& camera,
    const Player& player,
    float deltaTime,
    float worldWidth,
    int windowWidth
)
{
    Vector2 playerCenter =
    {
        player.position.x + player.size / 2.0f,
        player.position.y + player.size / 2.0f
    };

    const float smoothing = 5.0f;

    camera.target.x +=
        (playerCenter.x - camera.target.x) *
        smoothing * deltaTime;

    camera.target.y +=
        (playerCenter.y - camera.target.y) *
        smoothing * deltaTime;


    // Keep camera inside world bounds

    float halfScreenWidth =
        windowWidth / (2.0f * camera.zoom);

    if (worldWidth >= 2.0f * halfScreenWidth)
    {
        if (camera.target.x < halfScreenWidth)
        {
            camera.target.x = halfScreenWidth;
        }

        if (camera.target.x > worldWidth - halfScreenWidth)
        {
            camera.target.x = worldWidth - halfScreenWidth;
        }
    }
}


//****************************************
// DRAW PLAYER
//****************************************

void DrawPlayer(const Player& player)
{
    // The sprite sheet contains:
    //
    // Row 0:
    // Column 0 = Idle right
    // Columns 1-4 = Running right
    //
    // Row 1:
    // Column 0 = Idle left (not needed)
    // Columns 1-4 = Running left


    float frameWidth =
        player.spriteSheet.width / 5.0f;

    float frameHeight =
        player.spriteSheet.height / 2.0f;


    int column = 0;
    int row = 0;

    bool flipHorizontal = false;


    //****************************************
    // SELECT ANIMATION FRAME
    //****************************************

    if (player.animationState == AnimationState::RUNNING)
    {
        // Running frames occupy columns 1-4.

        column = player.currentFrame + 1;

        // Right-facing animation is row 0.
        // Left-facing animation is row 1.

        row = player.facingLeft ? 1 : 0;
    }
    else
    {
        // Use standing-right frame for idle,
        // jumping and falling until those animations
        // are added.

        column = 0;
        row = 0;

        // Mirror the standing frame when facing left.

        flipHorizontal = player.facingLeft;
    }


    //****************************************
    // SOURCE RECTANGLE
    //****************************************

    Rectangle source =
    {
        column * frameWidth,
        row * frameHeight,
        frameWidth,
        frameHeight
    };

    if (flipHorizontal)
    {
        source.x += frameWidth;

        source.width *= -1.0f;
    }


    //****************************************
    // DESTINATION RECTANGLE
    //****************************************

    // The sprite artwork is taller than the collision box.
    // Align its feet with the bottom of the player.

    const float drawWidth = 80.0f;
    const float drawHeight = 100.0f;

    Rectangle destination =
    {
        player.position.x + (player.size - drawWidth) / 2.0f,
        player.position.y + player.size - drawHeight,
        drawWidth,
        drawHeight
    };

    Vector2 origin = {0.0f, 0.0f};


    //****************************************
    // DRAW TEXTURE
    //****************************************

    DrawTexturePro(
        player.spriteSheet,
        source,
        destination,
        origin,
        0.0f,
        WHITE
    );
}


//****************************************
// DRAW WORLD
//****************************************

void DrawWorld(Platform platforms[], int platformCount)
{
    for (int i = 0; i < platformCount; i++)
    {
        DrawRectangleRec(
            platforms[i].rect,
            DARKGREEN
        );
    }
}


//****************************************
// DRAW HUD
//****************************************

void DrawHUD(int currentLevelIndex)
{
    DrawText(
        TextFormat("Level: %i", currentLevelIndex + 1),
        20,
        20,
        25,
        BLACK
    );

    DrawText(
        "Move: A/D or Arrow Keys",
        20,
        55,
        18,
        BLACK
    );

    DrawText(
        "Jump: SPACE",
        20,
        80,
        18,
        BLACK
    );

    DrawText(
        "Switch Levels: 1 / 2",
        20,
        105,
        18,
        BLACK
    );
}


//****************************************
// LOAD LEVEL
//****************************************

void LoadLevel(
    Player& player,
    Camera2D& camera,
    const Level& level
)
{
    // Reset position

    player.position = level.playerSpawn;


    // Reset velocity

    player.velocity = {0.0f, 0.0f};


    // Reset grounded state

    player.grounded = false;


    // Reset animation

    player.currentFrame = 0;

    player.animationTimer = 0.0f;

    player.animationState = AnimationState::IDLE;


    // Reset camera

    camera.target =
    {
        player.position.x + player.size / 2.0f,
        player.position.y + player.size / 2.0f
    };
}
