/*
 * ---------------------------------------------------
 *                                       .
 *                                     .o8
 * .oooooooo oooo  oooo  oo.ooooo.  .o888oo  .oooo.
 * 888' `88b  `888  `888   888' `88b   888   `P  )88b
 * 888   888   888   888   888   888   888    .oP"888
 * `88bod8P'   888   888   888   888   888 . d8(  888
 * `8oooooo.   `V88V"V8P'  888bod8P'   "888" `Y888""8o
 * d"     YD               888
 * "Y88888P'              o888o
 * ---------------------------------------------------
 * 3D rogue-lite with chess based movement.
 * Created for experimentaljams.com (Theme: GRID)
 */
#include "gupta.h"
#include <stdio.h>
#include <stdlib.h>

static const int screenWidth = 1280;
static const int screenHeight = 720;
int level = 1;
int rotation = 0;
int portalCount = 0;
bool playersTurn = true;
bool drawAITiles = false;
bool cameraOrbit = false;
bool cheatsheetOpen = false;
bool debug = false;
char debugText[50];

int enemyArrSize = 0;
Enemy* enemies;
Enemy enemyHover;
float hoverPosX;
float hoverPosY;


RenderTexture2D screenTexture;
Texture2D tileMarkerTex;
Texture2D tileJumpTex;
Texture2D bombTex;
Texture2D pickaxeTileTex;
Texture2D goldTex;
Texture2D playerTileTex;
Texture2D bg;
Texture2D cheatsheet;
Texture2D tntMod;
Texture2D picMod;
Texture2D logo;
Texture2D tank;
Texture2D floorTextures[LEVEL_COUNT];
Texture2D wallTextures[LEVEL_COUNT];
Texture2D backgrounds[LEVEL_COUNT];
Texture2D pathwayTextures[PATH_COUNT];

Sound moveSnd;
Sound selectSnd;
Sound switchSnd;
Sound goldSnd;
Sound enemyDeathSnd;
Sound playerDeathSnd;
Sound bombSnd;

Music bgMusic;
Music jungleMusic;
Music templeMusic;
Music snowMusic;
Music caveMusic;

Tile** map;
Player player;
Camera segmentCam;

Model mapModel;
Model pickaxeModel;
Model tntModel;
Model tntLabelModel;
Model altarModel;

Timer moveTimer;
Timer enemyMoveTimer;
Timer rotationTimer;

// function prototypes
static void start(void);       // Game variables are initialized here
static void update(void);  // Game Logic
static void render(void);  // Drawing/Rendering the game
void resetGame();
void updateLevel(void);  // Drawing/Rendering the game
static void cleanUp(void); // Cleanup, free allocated memory, close window, etc

Camera camera = { 0 };

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "gupta");
    SetTargetFPS(60); // lock game to 60 frames per second
    SetExitKey(KEY_NULL);
    InitAudioDevice();

    camera.position = (Vector3){ 0.0f, 20.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 30.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    segmentCam.position = (Vector3){ 0.0f, 0.0f, 10.0f };
    segmentCam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    segmentCam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    segmentCam.fovy = 10.0f;
    segmentCam.projection = CAMERA_ORTHOGRAPHIC;


    start();

    // main game loop
    while (!WindowShouldClose()) {
        update();
        render();
    }
    cleanUp();

    return 0;
}


void start(void) {
    /*
     * Init Game Resources
     */
    map = initMap(MAP_SIZE, MAP_SIZE);
    enemies = (Enemy*) calloc(1, sizeof(Enemy));
    if (enemies == NULL) {
        printf("Cannot allocate initial memory for data\n");
        exit(1);
    }

    mapModel = LoadModel("res/models/map.obj");
    mapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/model-textures/maptex.png");

    pickaxeModel = LoadModel("res/models/pickaxe.obj");
    pickaxeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/model-textures/pickaxe.png");

    tntModel = LoadModel("res/models/tnt.obj");
    //tntModel.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/model-textures/tntbase.png");
    tntModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/model-textures/tntbase.png");

    tntLabelModel = LoadModel("res/models/tnt-label.obj");
    tntLabelModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/model-textures/tnt.png");

    altarModel = LoadModel("res/models/altar.obj");
    altarModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/model-textures/altar.png");

    screenTexture = LoadRenderTexture(screenWidth, screenHeight);
    tileMarkerTex = LoadTexture("res/tile-x-alpha.png");
    tileJumpTex = LoadTexture("res/tile-jump-alpha.png");
    bombTex = LoadTexture("res/bomb.png");
    playerTileTex = LoadTexture("res/player-tile-alpha.png");
    cheatsheet = LoadTexture("res/bg/cheatsheet.png");
    tntMod = LoadTexture("res/modifiers/tnt.png");
    picMod = LoadTexture("res/modifiers/pickaxe.png");
    pickaxeTileTex = LoadTexture("res/mine-tile.png");

    for (int i = 0; i < PATH_COUNT; i++) {
        pathwayTextures[i] = LoadTexture(TextFormat("res/tiles/paths/%i.png", i));
    }

    goldTex = LoadTexture("res/tiles/special/gold.png");

    moveSnd = LoadSound("res/snd/move.wav");
    selectSnd = LoadSound("res/snd/select.wav");
    switchSnd = LoadSound("res/snd/switch.wav");
    goldSnd = LoadSound("res/snd/gold.wav");
    enemyDeathSnd = LoadSound("res/snd/enemy-death.wav");
    playerDeathSnd = LoadSound("res/snd/player-death.wav");
    bombSnd = LoadSound("res/snd/bomb.wav");

    templeMusic = LoadMusicStream("res/snd/music/temple.wav");
    caveMusic = LoadMusicStream("res/snd/music/ambience.wav");
    jungleMusic = LoadMusicStream("res/snd/music/jungle.wav");
    snowMusic = LoadMusicStream("res/snd/music/snow.wav");

    SetSoundVolume(selectSnd, 0.3f);
    SetSoundVolume(moveSnd, 0.3f);

    logo = LoadTexture("res/gupta.png");
    tank = LoadTexture("res/tank.png");
    player.pos = generateMap(map);
    generatePlayer(&player);
    player.selectedSegment = 0;

    for (int i = 0; i < LEVEL_COUNT; i++) {
        wallTextures[i] = LoadTexture(TextFormat("res/tiles/wall-%i.png", i));
        floorTextures[i] = LoadTexture(TextFormat("res/tiles/floor-%i.png", i));
        backgrounds[i] = LoadTexture(TextFormat("res/bg/%i.png", i));
    }

    updateLevel();
}


void update() {
    /*
     * Game Logic Loop
     */

    UpdateMusicStream(bgMusic);
    if (IsKeyPressed(KEY_ESCAPE)) {
        cheatsheetOpen = !cheatsheetOpen;
    }

    // rotation anim
    if (TimerDone(rotationTimer)) {
        startTimer(&rotationTimer, 0.05f);
        rotation++;
    }

    if (IsKeyPressed(KEY_INSERT)) {
        debug = !debug;
        if (!debug) {
            SetCameraMode(camera, CAMERA_PERSPECTIVE);
            camera.position = (Vector3){ 0.0f, 20.0f, 10.0f };
            camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.fovy = 30.0f;
        }
        else {
            SetCameraMode(camera, CAMERA_FREE);
        }
    }

    if (debug) {
        if (IsKeyPressed(KEY_E)){
            level += 1;
            updateLevel();
        }
        if (IsKeyPressed(KEY_M)) {
            player.moneys += 10000;
        }
    }

    UpdateCamera(&camera);

    if (player.dead && !cameraOrbit) {
        SetCameraMode(camera, CAMERA_ORBITAL);
        camera.target = (Vector3){-5,0,-1};

        cameraOrbit = true;
    }
    else if (player.dead && IsKeyPressed(KEY_ENTER)) {
        resetGame();
    }

    if (debug) {
        Ray ray = GetMouseRay(GetMousePosition(), camera);
        Ray raySegmentCam = GetMouseRay(GetMousePosition(), segmentCam);
        for (int y = player.pos.y - MAP_SIZE; y < player.pos.y + MAP_SIZE; y++) {
            for (int x = player.pos.x - MAP_SIZE; x < player.pos.x + MAP_SIZE; x++) {
                if (x >= MAP_SIZE || y >= MAP_SIZE || x < 0 || y < 0) {
                    continue;
                }
                RayCollision collision = GetRayCollisionBox(ray, map[y][x].col);
                if (collision.hit) {
                    map[y][x].selectionColor = GREEN;
                    if (IsMouseButtonPressed(0) && TimerDone(moveTimer)) {
                        startTimer(&moveTimer, 0.01f);
                        player.pos.x = x;
                        player.pos.y = y;
                    }
                }
                else {
                    map[y][x].selectionColor = WHITE;
                }
            }
        }
    }

    if (!player.dead && !debug) {
        /* for debug remove later */
        //int mouseX = (int) GetMousePosition().x;
        //int mouseY = (int) GetMousePosition().y;
        //sprintf(debugText, "(%i, %i)", mouseX, mouseY);
        /* ---------------------- */

        Ray ray = GetMouseRay(GetMousePosition(), camera);
        Ray raySegmentCam = GetMouseRay(GetMousePosition(), segmentCam);

        processHover(ray, map, &player);

        if (IsKeyPressed(KEY_ONE)) {
            PlaySound(selectSnd);
            player.selectedSegment = 0;
        }
        else if (IsKeyPressed(KEY_TWO)) {
            PlaySound(selectSnd);
            player.selectedSegment = 1;
        }
        else if (IsKeyPressed(KEY_THREE)) {
            PlaySound(selectSnd);
            player.selectedSegment = 2;
        }

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            switchSegment(&player, -1);
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            switchSegment(&player, 1);
        }

        if (IsKeyPressed(KEY_SPACE)) {
            int x = (int) player.pos.x;
            int y = (int) player.pos.y;

            if (map[y][x].loot) {
                Segment temp = player.segments[player.selectedSegment];
                player.segments[player.selectedSegment] = map[y][x].segment;
                map[y][x].segment = temp;
                updateMovePattern(&player);
            }
            if (map[y][x].pickaxe && player.moneys >= 1000) {
                player.moneys -= 1000;
                player.segments[2].moveType = 3;
                updateMovePattern(&player);
                map[y][x].pickaxe = false;
            }

            if (map[y][x].bombs && player.moneys >= 10000) {
                player.moneys -= 10000;
                player.segments[2].moveType = 4;
                updateMovePattern(&player);
                map[y][x].bombs = false;
            }

            if (map[y][x].map && player.moneys >= 5000 && !player.hasMap) {
                player.moneys -= 5000;
                player.hasMap = true;
                map[y][x].map = false;
            }

        }
    }
}


void render() {
    /*
     * Draw sprites to the screen
     */

    BeginTextureMode(screenTexture);

    BeginMode3D(camera);
        ClearBackground((Color){0,0,0,0});
        if (!player.dead && !debug) {
            DrawCubeTexture(bg, (Vector3){0,-3.3,-2.65},28, 1, 16, WHITE);
        }
        drawMapArea(player, map, MAP_SIZE, MAP_SIZE);
        if (drawAITiles) {
            drawEnemyTiles(enemyHover, 0, map, hoverPosX, hoverPosY);
        }
    EndMode3D();

    if (!player.dead && !debug) {
        DrawTexture(tank, 609, 160, WHITE);
        if (player.hasMap)
            drawMiniMap(map, enemies, enemyArrSize, player, 621, 25);

        BeginMode3D(segmentCam);
            Vector3 segmentsPos = {1.35,-1.3,0};
            drawPlayerSegments(player, segmentsPos, 1);
            int playerX = (int)player.pos.x;
            int playerY = (int)player.pos.y;
            if (map[playerY][playerX].loot) {
                Vector2 lootPos = {5.5,2.3};
                Vector2 infoPos = {1000,328};
                drawSegmentInfo(map[playerY][playerX].segment, segmentCam, lootPos, infoPos, 2);
            }
            if (map[playerY][playerX].bombs || map[playerY][playerX].pickaxe || map[playerY][playerX].map) {
                Vector2 lootPos = {5.5,2.3};
                Vector2 infoPos = {1036,351};
                drawShopItemInfo(map[playerY][playerX], segmentCam, lootPos, infoPos);
            }
        EndMode3D();

        DrawTexture(logo, 219, 30, WHITE);
        DrawText(debugText, 40, screenHeight - 40, 40, WHITE);

        drawPattern(player.segments[0].movePattern, 794, 436, 4);
        if (player.segments[2].combine) {
            drawPattern(player.segments[1].movePattern, 794, 372, 4);
        }

        if (player.segments[2].combine) {
            DrawRing((Vector2){800, 310}, 5, 8, 0, 360, 20, ORANGE);
        }
        if (player.segments[2].moveType == 2) {
            DrawCircle(800, 290, 8, BLUE);
        }
        else if (player.segments[2].moveType == 3) {
            DrawTexture(picMod, 793,284, WHITE);
        }
        else if (player.segments[2].moveType == 4) {
            DrawTexture(tntMod, 793, 284, WHITE);
        }
        else {
            DrawCircle(800, 290, 8, RED);
        }

        for (int i = 0; i < player.segments[1].movelength; i++) {
            DrawRectangle(676, 400 - (i * 9), 8, 8, GREEN);
        }

    }
    else if (player.dead) {
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0,0,0,75});
        DrawText("You Died", screenWidth/2-80, 40, 40, WHITE);
        DrawText("Press Enter to Restart", screenWidth/2-240, 80, 40, WHITE);
        drawStats(player, 402, 143);
    }
    else if (debug) {
        DrawText("DEBUG", screenWidth/2-80, 40, 40, SKYBLUE);
    }
    DrawText(TextFormat("$%i", player.moneys), 615, 545, 40, ORANGE);

    if (cheatsheetOpen) {
        DrawTexture(cheatsheet, 0, 0, WHITE);
    }

    EndTextureMode();

    BeginDrawing();

        float scale = MIN(
            (float)GetScreenWidth()/screenWidth,
            (float)GetScreenHeight()/screenHeight
        );
        float centerW = (GetScreenWidth() - ((float)screenWidth*scale))*0.5f;
        float centerH = (GetScreenHeight() - ((float)screenHeight*scale))*0.5f;
        float drawW = (float)screenWidth*scale;
        float drawH = (float)screenHeight*scale;

        int w = screenTexture.texture.width;
        int h = -screenTexture.texture.height;


        DrawTexturePro(
            screenTexture.texture,
            (Rectangle){0, 0, w, h},
            (Rectangle){centerW, centerH, drawW, drawH},
            (Vector2){0,0}, 0, WHITE
        );
        ClearBackground(BLACK);
        //DrawFPS(0, 0);
    EndDrawing();
}


void updateLevel(void) {
    free(enemies);
    enemies = (Enemy*) calloc(1, sizeof(Enemy));
    enemyArrSize = 0;
    //unloadMap(map, MAP_SIZE);

    //map = initMap(MAP_SIZE, MAP_SIZE);
    resetMap(map, MAP_SIZE, MAP_SIZE);
    if (level % 2 == 0) {
        bg = backgrounds[0];
        player.pos = generateShop(map);
    }
    else {
        int wall = GetRandomValue(1, LEVEL_COUNT - 1);
      //int bgIdx = GetRandomValue(1, LEVEL_COUNT - 1);
        setWallTexture(map, wall);
        bg = backgrounds[wall];
        player.pos = generateMap(map);

        switch(wall) {
            case 1: bgMusic = jungleMusic; break;
            case 2: bgMusic = snowMusic; break;
            case 3: bgMusic = templeMusic; break;
            case 4: bgMusic = caveMusic; break;
        }
        PlayMusicStream(bgMusic);
    }
}

void resetGame() {
    SetCameraMode(camera, CAMERA_PERSPECTIVE);
    camera.position = (Vector3){ 0.0f, 20.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 30.0f;
    generatePlayer(&player);
    player.moneys = 0;
    player.dead = false;
    player.kills = 0;
    player.moves = 0;
    level = 1;
    updateLevel();
}


void cleanUp(void) {
    unloadMap(map, MAP_SIZE);
    free(enemies);

    /* Unload Textures */
    UnloadRenderTexture(screenTexture);
    UnloadTexture(tileMarkerTex);
    UnloadTexture(tileJumpTex);
    UnloadTexture(playerTileTex);
    UnloadTexture(bombTex);
    UnloadTexture(bg);
    UnloadTexture(logo);
    UnloadTexture(tank);
    UnloadTexture(goldTex);
    UnloadTexture(cheatsheet);
    UnloadTexture(tntMod);
    UnloadTexture(picMod);
    UnloadTexture(pickaxeTileTex);

    for (int i = 0; i < LEVEL_COUNT; i++) {
        UnloadTexture(floorTextures[i]);
        UnloadTexture(wallTextures[i]);
        UnloadTexture(backgrounds[i]);
        UnloadTexture(pathwayTextures[i]);
    }

    /* Unload Sounds */
    UnloadSound(moveSnd);
    UnloadSound(selectSnd);
    UnloadSound(switchSnd);
    UnloadSound(goldSnd);
    UnloadSound(enemyDeathSnd);
    UnloadSound(playerDeathSnd);
    UnloadSound(bombSnd);

    /* Unload Models */
    UnloadModel(mapModel);
    UnloadModel(tntModel);
    UnloadModel(tntLabelModel);
    UnloadModel(pickaxeModel);
    UnloadModel(altarModel);

    CloseWindow();
}
