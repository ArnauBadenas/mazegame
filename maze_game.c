/*******************************************************************************************
*
*   raylib maze game
*
*   Procedural maze generator using Maze Grid Algorithm
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2024-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include <stdlib.h>     // Required for: malloc(), free()
#include <math.h>
#include <stdio.h>

#define MAZE_WIDTH          64
#define MAZE_HEIGHT         64
#define MAZE_SCALE          10.0f

#define MAX_MAZE_ITEMS      16

// Declare new data type: Point
typedef struct Point {
    int x;
    int y;
} Point;

// Generate procedural maze image, using grid-based algorithm
// NOTE: Functions defined as static are internal to the module
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Delivery04 - maze game");
    
    // Initialize audio device
    InitAudioDevice();
    
    Music backgroundMusic = LoadMusicStream("resources/background_music.ogg");
    PlayMusicStream(backgroundMusic);
    
    Sound itemPickUpSound = LoadSound("resources/item_pick_up.wav");
    Sound winSound = LoadSound("resources/win_sound.ogg");
    
    // Declare and initialize custom font
    Font customFont = LoadFontEx("resources/ari_w9500_condensed_display.ttf", 40, 0, 250);

    // Current application mode
    int currentMode = 1;    // 0-Game, 1-Editor, 2-Ending

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    unsigned int seed = 123456;
    SetRandomSeed(seed);

    // Generate maze image using the grid-based generator
    // TODO: [1p] Implement GenImageMaze() function with required parameters
    Image imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);

    // Load a texture to be drawn on screen from our image data
    // WARNING: If imMaze pixel data is modified, texMaze needs to be re-loaded
    Texture texMaze = LoadTextureFromImage(imMaze);

    // Player start-position and end-position initialization
    Point startCell = { 1, 1 };
    Point endCell = { imMaze.width - 2, imMaze.height - 2 };

    // Maze drawing position (editor mode)
    Vector2 mazePosition = {
        GetScreenWidth()/2 - texMaze.width*MAZE_SCALE/2,
        GetScreenHeight()/2 - texMaze.height*MAZE_SCALE/2
    };
        
    Point endPoint = { endCell.x, endCell.y };
    ImageDrawPixel(&imMaze, endPoint.x, endPoint.y, GREEN);
    
    UnloadTexture(texMaze);
    texMaze = LoadTextureFromImage(imMaze);

    // Define player position and size
    Rectangle player = { mazePosition.x + 1*MAZE_SCALE + 128, mazePosition.y + 1*MAZE_SCALE + 128, 50, 50 };
    float playerSpeed = 300.0f;
    
    Point playerCell;
    playerCell.x = startCell.x;
    playerCell.y = startCell.y;
    
    Rectangle playerBounds[8] = { 0 };   

    // Camera 2D for 2d gameplay mode
    // TODO: [2p] Initialize camera parameters as required
    Camera2D camera2d = { 0 };
    camera2d.target = (Vector2){ player.x + 20.0f, player.y - 20.0f };
    camera2d.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera2d.zoom = 1.0f;

    // Mouse selected cell for maze editing
    Point selectedCell = { 0 };
    Point mouseCell = { 0 };

    // Maze items position and state
    Point mazeItems[MAX_MAZE_ITEMS] = { 0 };
    bool mazeItemPicked[MAX_MAZE_ITEMS] = { 0 };
    int mazeItemCount = 0;
    
    int score = 0;
    
    // Define textures to be used as our "biomes"
    Texture texBiomes[4] = { 0 };
    texBiomes[0] = LoadTexture("resources/maze_atlas01.png");
    // TODO: Load additional textures for different biomes
    texBiomes[1] = LoadTexture("resources/maze_atlas02.png");
    texBiomes[2] = LoadTexture("resources/maze_atlas03.png");
    texBiomes[3] = LoadTexture("resources/maze_atlas04.png");
    
    Texture texItem = LoadTexture("resources/coin.png");
    Texture texPlayer = LoadTexture("resources/egg_player.png");
    
    int currentBiome = 2;

    // TODO: Define all variables required for game UI elements (sprites, fonts...)
    
    //Image for final scene (when game ends)
    Image imFinal = LoadImage("resources/image_final_scene.png");
    ImageResize(&imFinal, screenWidth, screenHeight);
    
    Texture texFinal = LoadTextureFromImage(imFinal);
    
    // Set our game to run at 60 frames-per-second
    SetTargetFPS(60); 
    
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Select current mode as desired
        UpdateMusicStream(backgroundMusic);
        
        if (IsKeyPressed(KEY_SPACE)) currentMode = !currentMode; // Toggle mode: 0-Game, 1-Editor
        
        if (IsKeyPressed(KEY_R))
        {
            seed++;
            SetRandomSeed(seed);
            
            imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);
            
            endPoint.x = endCell.x;
            endPoint.y = endCell.y;
            
            ImageDrawPixel(&imMaze, endPoint.x, endPoint.y, GREEN);
            
            UnloadTexture(texMaze);
            texMaze = LoadTextureFromImage(imMaze);
        }

        if (currentMode == 0) // Game mode
        {
            // TODO: [2p] Player 2D movement from predefined Start-point to End-point
            // Implement maze 2D player movement logic (cursors || WASD)
            // Use imMaze pixel information to check collisions
            // Detect if current playerCell == endCell to finish game

            Rectangle prevPlayer = player;
            
            const Point offsets[8] = {
                { 0, -1}, { 1,  0}, { 0,  1}, {-1,  0},
                {-1, -1}, { 1, -1}, { 1,  1}, {-1,  1}
            };
            
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) player.y -= (playerSpeed * GetFrameTime());
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) player.y += (playerSpeed * GetFrameTime());
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) player.x -= (playerSpeed * GetFrameTime());
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.x += (playerSpeed * GetFrameTime());
            
            playerCell.x = (int)((player.x + player.width / 2 - mazePosition.x) / ((float)texBiomes[currentBiome].width * 0.5f));
            playerCell.y = (int)((player.y + player.height / 2 - mazePosition.y) / ((float)texBiomes[currentBiome].height * 0.5f));
            
            bool hitWall = false;
            bool hitWin = false;
            
            for (int i = 0; i < 8; i++) 
            {
                // Calculate grid coordinates for the cell that we are going to check
                int checkX = playerCell.x + offsets[i].x;
                int checkY = playerCell.y + offsets[i].y;

                // Calculate this cell's rectangle
                playerBounds[i] = (Rectangle){ 
                    mazePosition.x + checkX * 128.0f, 
                    mazePosition.y + checkY * 128.0f, 
                    128.0f, 128.0f 
                };

                if (CheckCollisionRecs(player, playerBounds[i])) 
                {
                    // Check pixel color on current cell
                    Color cellColor = GetImageColor(imMaze, checkX, checkY);
                    
                    if (ColorIsEqual(cellColor, WHITE)) hitWall = true;
                    if (ColorIsEqual(cellColor, GREEN)) hitWin = true;
                }
            }

            if (hitWall) 
            {
                player = prevPlayer; 
            }
            else if (hitWin)
            {
                PlaySound(winSound);
                currentMode = 2;
            }

            // TODO: [1p] Camera 2D system following player movement around the map
            // Update Camera2D parameters as required to follow player and zoom control
            camera2d.target = (Vector2){ player.x + 20.0f, player.y - 20.0f };
            camera2d.zoom = expf(logf(camera2d.zoom) + ((float)GetMouseWheelMove() * 0.1f));
            
            if (camera2d.zoom > 3.0f) camera2d.zoom = 3.0f;
            else if (camera2d.zoom < 0.1f) camera2d.zoom = 0.1f;

            // TODO: [2p] Maze items pickup logic
            for (int i = 0; i < mazeItemCount; i++)
            {
                if (!mazeItemPicked[i])
                {
                    if (playerCell.x == mazeItems[i].x && playerCell.y == mazeItems[i].y)
                    {
                        mazeItemPicked[i] = true;
                        PlaySound(itemPickUpSound);
                        score++;
                        
                        ImageDrawPixel(&imMaze, mazeItems[i].x, mazeItems[i].y, BLACK);
                        
                        UnloadTexture(texMaze);
                        texMaze = LoadTextureFromImage(imMaze);
                    }
                }
            }
        }
        else if (currentMode == 1) // Editor mode
        {
            // TODO: [2p] Maze editor mode, edit image pixels with mouse.
            // Implement logic to selecte image cell from mouse position -> TIP: GetMousePosition()
            // NOTE: Mouse position is returned in screen coordinates and it has to 
            // transformed into image coordinates
            // Once the cell is selected, if mouse button pressed add/remove image pixels
            
            // WARNING: Remember that when imMaze changes, texMaze must be also updated!
            
            mouseCell.x = GetMouseX();
            mouseCell.y = GetMouseY();
            
            selectedCell.x = (mouseCell.x - mazePosition.x) / MAZE_SCALE;
            selectedCell.y = (mouseCell.y - mazePosition.y) / MAZE_SCALE;
            
            if ((selectedCell.x >= 0) && (selectedCell.y >= 0) && (selectedCell.x < imMaze.width) && (selectedCell.y < imMaze.height))
            {
                if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                    ImageDrawPixel(&imMaze, endPoint.x, endPoint.y, BLACK);
                    
                    endPoint.x = selectedCell.x;
                    endPoint.y = selectedCell.y;
                    
                    ImageDrawPixel(&imMaze, endPoint.x, endPoint.y, GREEN);
                    
                    UnloadTexture(texMaze);
                    texMaze = LoadTextureFromImage(imMaze);
                }
                else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, WHITE);
                    
                    UnloadTexture(texMaze);
                    texMaze = LoadTextureFromImage(imMaze);
                }
                else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                    ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, BLACK);
                    
                    UnloadTexture(texMaze);
                    texMaze = LoadTextureFromImage(imMaze);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
                {
                    if (mazeItemCount < MAX_MAZE_ITEMS)
                    {
                        mazeItems[mazeItemCount] = (Point){ selectedCell.x, selectedCell.y };
                        mazeItemPicked[mazeItemCount] = false;
                        mazeItemCount++;
                        
                        ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, RED);
                        
                        UnloadTexture(texMaze);
                        texMaze = LoadTextureFromImage(imMaze);
                    }
                }
            }

            // TODO: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use mazeItems[]
        }

        // TODO: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material        
        if (IsKeyPressed(KEY_ONE)) currentBiome = 0;
        if (IsKeyPressed(KEY_TWO)) currentBiome = 1;
        if (IsKeyPressed(KEY_THREE)) currentBiome = 2;
        if (IsKeyPressed(KEY_FOUR)) currentBiome = 3;

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (currentMode == 0) // Game mode
            {
                // Draw maze using camera2d (for automatic positioning and scale)
                BeginMode2D(camera2d);
           
                    // TODO: Draw maze walls and floor using current texture biome 
                    for (int y = 0; y < imMaze.height; y++)
                    {
                        for (int x = 0; x < imMaze.width; x++)
                        {
                            if (ColorIsEqual(GetImageColor(imMaze, x, y), WHITE))
                            {
                                DrawTextureRec(texBiomes[currentBiome], (Rectangle) { texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2, texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2 }, (Vector2) { mazePosition.x + x * texBiomes[currentBiome].width / 2, mazePosition.y + y * texBiomes[currentBiome].height / 2 }, WHITE);
                            }
                            else if (ColorIsEqual(GetImageColor(imMaze, x, y), BLACK))
                            {
                                DrawTextureRec(texBiomes[currentBiome], (Rectangle) { 0, 0, texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2 }, (Vector2) { mazePosition.x + x * texBiomes[currentBiome].width / 2, mazePosition.y + y * texBiomes[currentBiome].height / 2 }, WHITE);
                            }
                            else if (ColorIsEqual(GetImageColor(imMaze, x, y), GREEN))
                            {
                                DrawTextureRec(texBiomes[currentBiome], (Rectangle) { 0, texBiomes[currentBiome].height / 2, texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2 }, (Vector2) { mazePosition.x + x * texBiomes[currentBiome].width / 2, mazePosition.y + y * texBiomes[currentBiome].height / 2 }, WHITE);
                            }
                            else if (ColorIsEqual(GetImageColor(imMaze, x, y), RED))
                            {
                                // TODO: Draw maze items 2d (using sprite texture?)
                                DrawTextureRec(texBiomes[currentBiome], (Rectangle) { 0, 0, texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2 }, (Vector2) { mazePosition.x + x * texBiomes[currentBiome].width / 2, mazePosition.y + y * texBiomes[currentBiome].height / 2 }, WHITE);

                                DrawTexture(texItem, mazePosition.x + x * (texBiomes[currentBiome].width / 2), mazePosition.y + y * (texBiomes[currentBiome].height / 2), WHITE);
                            }
                        }
                    }
             
                    // TODO: Draw player rectangle or sprite at player position
                    DrawTexture(texPlayer,player.x,player.y,WHITE);

                EndMode2D();

                // TODO: Draw game UI (score, time...) using custom sprites/fonts
                // NOTE: Game UI does not receive the camera2d transformations,
                // it is drawn in screen space coordinates directly    
                DrawTextureEx(texMaze, (Vector2) { GetScreenWidth() - texMaze.width * 4.0f - 10, 10 }, 0.0f, 4.0f, WHITE);
                DrawRectangle(GetScreenWidth() - texMaze.width * 4.0f - 10 + playerCell.x * 4.0f, 10 + playerCell.y * 4.0f, 4, 4, BLUE);
                
                DrawRectangle(0, 35, MeasureText("SCORE: AAAA", 40), 40, WHITE);
                DrawTextEx(customFont, TextFormat("SCORE: %02i", score), (Vector2){22.0f,42.0f}, 35, 1, BLACK);
                
            }
            else if (currentMode == 1) // Editor mode
            {
                // Draw generated maze texture, scaled and centered on screen 
                DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);

                // Draw lines rectangle over texture, scaled and centered on screen 
                DrawRectangleLines(mazePosition.x, mazePosition.y, MAZE_WIDTH*MAZE_SCALE, MAZE_HEIGHT*MAZE_SCALE, RED);

                // TODO: Draw player using a rectangle, consider maze screen coordinates!
                DrawRectangle(mazePosition.x + playerCell.x * MAZE_SCALE, mazePosition.y + playerCell.y * MAZE_SCALE, MAZE_SCALE, MAZE_SCALE, BLUE);
                // TODO: Draw editor UI required elements
                
            }
            else if (currentMode == 2)
            {             
               DrawTexture(texFinal, screenWidth/2 - texFinal.width/2, screenHeight/1.3 - texFinal.height/1.3, WHITE);
               DrawRectangle(screenWidth/2 - MeasureText("CONGRATULATIONS", 70)/2 - 15, 0, MeasureText("CONGRATULATIONS", 70) + 30, 70, PINK);
               DrawTextEx(customFont,"CONGRATULATIONS", (Vector2){screenWidth/2 - MeasureText("CONGRATULATIONS", 70)/2, 5.0f}, 70, 1, BLACK);
            }

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texMaze);     // Unload maze texture from VRAM (GPU)
    UnloadImage(imMaze);        // Unload maze image from RAM (CPU)
    
    UnloadTexture(texFinal);
    UnloadImage(imFinal);
    
    // TODO: Unload all loaded resources
    for (int i = 0; i < 4; i++) UnloadTexture(texBiomes[i]);
    
    UnloadMusicStream(backgroundMusic);
    UnloadSound(itemPickUpSound);
    UnloadSound(winSound);
    
    UnloadFont(customFont);
    
    CloseAudioDevice();
    
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    Image imMaze = { 0 };
    
    // TODO: [1p] Implement maze image generation algorithm
    imMaze = GenImageColor(width, height, BLACK);
    
    Point* mazePoints = malloc(64*sizeof(Point));
    int mazePointCounter = 0;
    
    for (int y = 0; y < imMaze.width; y++)
    {
        for (int x = 0; x < imMaze.height; x++)
        {
            if ((x == 0) || (y == 0) || (x == (imMaze.width - 1)) || (y == (imMaze.height - 1)))
            {
                ImageDrawPixel(&imMaze, x, y, WHITE);
            }
            else if ((x % 4 == 0) && (y % 4 == 0))
            {
                if (GetRandomValue(0, 10) < pointChance)
                {
                    ImageDrawPixel(&imMaze, x, y, WHITE);
                    
                    mazePoints[mazePointCounter].x = x;
                    mazePoints[mazePointCounter].y = y;
                    mazePointCounter++;
                }
            }
        }
    }
    
    Point dirIncrementals[4] = {
        { 0, -1 },
        { 1, 0 },
        { 0, 1 },
        { -1, 0 }
    };
    
    int* indices = LoadRandomSequence(mazePointCounter, 0, mazePointCounter - 1);
    
    for (int i = 0; i < mazePointCounter; i++)
    {
        int dir = GetRandomValue(0, 3);
        
        Point nextMazePoint = mazePoints[indices[i]];        
        nextMazePoint.x += dirIncrementals[dir].x;
        nextMazePoint.y += dirIncrementals[dir].y;
        
        while (ColorIsEqual(GetImageColor(imMaze, nextMazePoint.x, nextMazePoint.y), BLACK))
        {
            ImageDrawPixel(&imMaze, nextMazePoint.x, nextMazePoint.y, WHITE);
            
            nextMazePoint.x += dirIncrementals[dir].x;
            nextMazePoint.y += dirIncrementals[dir].y;
        }
    }
    
    UnloadRandomSequence(indices);
    free(mazePoints);
    
    return imMaze;
}
