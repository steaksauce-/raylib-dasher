#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;

        // Update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    // Window Dimensions
    const int window_width{512};
    const int window_height{380};

    // Initialize the window
    InitWindow(window_width, window_height, "Raylib Dasher Exmaple");

    // Acceleration from Gravity (pixels/second)/second
    const int gravity{1'000};
    const int jumpVel{-600}; // pixels per second jump velocity
    bool isInAir{};

    // Nebula Variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{2};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i=0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = window_height - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0 / 16.0;
        nebulae[i].pos.x = window_width + (300 * i);
    }

    float finishLine { nebulae[sizeOfNebulae - 1].pos.x };

    // Nebula x velocity in pixels per second
    const int nebVel{-200};

    // Scarfy Variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    AnimData scarfyData{ 
        {0.0, 0.0, scarfy.width/6, scarfy.height}, 
        {window_width/2 - scarfyData.rec.width/2, window_height - scarfyData.rec.height}, 
        0, 
        1.0 / 12.0, 
        0};

    int velocity{0};

    // Background Texture
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    // Midground Texture
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};

    // Foreground Texture
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    // collision
    bool collision{};

    // We want to set this, so the game doesn't go brrrr faster than we can see
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Delta Time (time since last frame)
        const float dT{GetFrameTime()};

        // Start Drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // Draw Background and pan it
        bgX -= 20 * dT;
        if (bgX <= -background.width * 2)
        {
            // Reposition to keep scrolling.
            bgX = 0.0;
        }

        Vector2 bg1Pos{bgX, 0.0};
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // Draw midground and pan it
        mgX -= 40 * dT;
        if (mgX <= -midground.width * 2)
        {
            // Reposition to keep scrolling.
            mgX = 0.0;
        }

        Vector2 mg1Pos{mgX, 0.0};
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // Draw foreground and pan it
        fgX -= 40 * dT;
        if (fgX <= -foreground.width * 2)
        {
            // Reposition to keep scrolling.
            fgX = 0.0;
        }

        Vector2 fg1Pos{fgX, 0.0};
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        // Ground Check
        if (isOnGround(scarfyData, window_height))
        {
            // Rectangle is on the ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // Apply Gravity
            velocity += gravity * dT;
            isInAir = true;          
        }

        // Check for jumping
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
        }

        // Update Scarfy Position
        scarfyData.pos.y += velocity * dT;

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad,
            };
            
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        // This updates the Nebula position, as well as frame based on time
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // Update Nebula Position
            nebulae[i].pos.x += nebVel * dT;
            // Update frame
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        // Update finishLine Position
        finishLine += nebVel * dT;

        // This prevents Scarfy from updating each frame (which would have been targeted at 60 frames per second)
        // This also prevents Scarfy from animating in the air
        if (!isInAir)
        {
            // Update Scarfy's Sprite Frame
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }
        
        if (collision)
        {
            // We lose
            DrawText("Game Over", window_width / 4, window_height / 2, 40, RED);
        }
        else if (scarfyData.pos.x >= finishLine)
        {
            DrawText("You Win!", window_width / 4, window_height / 2, 40, RED);
        }
        else
        {
            // Draw Nebula
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            // Draw Scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        // End Drawing
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}