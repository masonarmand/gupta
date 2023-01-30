/*
 * draw.c
 * Functions for drawing game elements to the screen.
 */
#include "gupta.h"
#include <math.h>
#include <rlgl.h>

// function prototypes
void drawSegment(Segment segment, Vector3 pos, float size, int sIdx, bool wires, bool selected, bool spinning);

void drawPlayer(Player player, Vector2 pos, float size) {
    if (!player.dead) {
        for (int sIdx = 0; sIdx < 3; sIdx++) {
            Vector3 pos3D = {pos.x, 0, pos.y};
            drawSegment(player.segments[sIdx], pos3D, size, sIdx, false, false, false);
        }
    }
}

void drawEnemies(Enemy* enemies, int enemyArrSize, Player player, float size, int x, int y) {
    for (int i = 0; i < enemyArrSize; i++) {
        if (enemies[i].pos.x == x && enemies[i].pos.y == y && !enemies[i].dead) {
            for (int sIdx = 0; sIdx < 3; sIdx++) {
                Vector2 pos = { (x-6) - player.pos.x + 1, (y-1.5) - player.pos.y + 1};
                Vector3 pos3D = { pos.x, 0, pos.y };
                drawSegment(enemies[i].segments[sIdx], pos3D, size, sIdx, false, false, false);
            }
        }
    }
}

void drawTile(Tile tile, Tile** map, float size, Vector2 pos, Vector2 patternPos, Vector2 mapPos, bool isPlayer, Player player) {
    Vector3 tile3Dpos = { pos.x, 0, pos.y };
    float height;

    Texture2D tileTex;

    switch (tile.type) {
        case FLOOR:
            height = 0.1;
            tile3Dpos.y = -0.55f;
            tileTex = floorTextures[tile.texId];
        break;
        case PATH:
            height = 0.1;
            tile3Dpos.y = -0.55f;
            tileTex = pathwayTextures[tile.texId];
        break;
        case ALTAR:
            height = 0.1;
            tile3Dpos.y = -0.55f;
            tileTex = floorTextures[tile.texId];
        break;
        case WALL:
            height = size;
            tileTex = wallTextures[tile.texId];
        break;
        case PORTAL:
            height = 0.1;
            tile3Dpos.y = -0.6f;
            tileTex = floorTextures[tile.texId];
        break;
    }

    if (tile.goldblock) {
        tileTex = goldTex;
    }

    int moveType = player.movePattern[(int)patternPos.y][(int)patternPos.x];

    Color color = tile.color;

    if (tile.selectionColor.r != 255 &&
        tile.selectionColor.b != 255 &&
        tile.selectionColor.g != 255)
    {
        color = tile.selectionColor;
    }


    if (tile.type == PORTAL) {
        rlPushMatrix();
            Vector3 portalPos = { pos.x, 0.5f, pos.y };
            rlRotatef(rotation, portalPos.x, portalPos.y, portalPos.z);
            DrawSphereWires(portalPos, 0.5f, 6, 6, PURPLE);
        rlPopMatrix();
    }
    else if (tile.type == ALTAR) {
        Vector3 altarPos = { pos.x, 0.f, pos.y };
        DrawModel(altarModel, altarPos, 1.6f, WHITE);
    }

    if (tile.loot) {
        drawSegment(tile.segment, tile3Dpos, 0.5f, 1, false, false, true);
    }

    if (tile.map) {
        rlPushMatrix();
            Vector3 mapPos = { pos.x, 0.3f, pos.y };
            rlRotatef(rotation, mapPos.x, mapPos.y, mapPos.z);
            //DrawSphereWires(portalPos, 0.5f, 6, 6, PURPLE);
            DrawModel(mapModel, mapPos, 0.5f, WHITE);
        rlPopMatrix();
    }
    if (tile.pickaxe) {
        rlPushMatrix();
            Vector3 pickaxePos = { pos.x, 0.5f, pos.y +0.5f};
            rlRotatef(rotation, pickaxePos.x, pickaxePos.y, pickaxePos.z);
            DrawModel(pickaxeModel, pickaxePos, 0.5f, WHITE);
            DrawModelWires(pickaxeModel, pickaxePos, 0.5f, BLACK);
        rlPopMatrix();
    }
    if (tile.bombs) {
        rlPushMatrix();
            Vector3 tntPos = { pos.x, 0.5f, pos.y +0.5f};
            rlRotatef(rotation, tntPos.x, tntPos.y, tntPos.z);
            DrawModel(tntModel, tntPos, 0.4f, WHITE);
            DrawModel(tntLabelModel, tntPos, 0.4f, WHITE);
            DrawModelWires(tntModel, tntPos, 0.4f, BLACK);
        rlPopMatrix();
    }

    if ((!isFloor(tile.type) && !debug && !player.dead) || isFloor(tile.type)) {
        DrawCubeTexture(tileTex, tile3Dpos, size, height, size, color);
    }

    if (!player.dead && !debug) {
        Vector3 markerPos = tile3Dpos;
        markerPos.y += 0.04f;

        if (tile.type != WALL && moveType == 1 && canMove(player, map, patternPos, mapPos)) {
            DrawCubeTexture(tileMarkerTex, markerPos, size, 0.02f, size, tile.selectionColor);
        }

        else if (tile.type != WALL && moveType == 2) {
            DrawCubeTexture(tileJumpTex, markerPos, size, 0.02f, size, tile.selectionColor);
        }
        else if (isPlayer && tile.type != WALL) {
            DrawCubeTexture(playerTileTex, markerPos, size, 0.02f, size, tile.selectionColor);
        }
        else if (moveType == 3) {
            if (tile.type == WALL) {
                markerPos.y = tile3Dpos.y + 0.5f;
            }
            DrawCubeTexture(pickaxeTileTex, markerPos, size, 0.02f, size, tile.selectionColor);
        }
        else if (moveType == 4) {
            if (tile.type == WALL) {
                markerPos.y = tile3Dpos.y + 0.5f;
            }
            DrawCubeTexture(bombTex, markerPos, size, 0.02f, size, tile.selectionColor);
        }
    }
    else if (mapPos.x == player.killer.pos.x && mapPos.y == player.killer.pos.y && !debug) {
        drawEnemyTiles(player.killer, 1, map, pos.x, pos.y);
    }

    if (tile.gold > 0) {
        Vector3 goldPos = {pos.x, -0.4f, pos.y};
        DrawCylinder(goldPos, 0.1f, 0.1f, 0.1f, 10, YELLOW);
        DrawCylinderWires(goldPos, 0.1f, 0.1f, 0.1f, 10, BLACK);
    }
}

void drawMapArea(Player player, Tile** map, int width, int height) {
    int bound1 = 4;
    int bound2 = 5;

    if (player.dead || debug) {
        bound1 = 80;
        bound2 = 80;
    }

    for (int y = player.pos.y - bound1; y < player.pos.y + bound2; y++) {
        for (int x = player.pos.x - bound1; x < player.pos.x + bound2; x++) {
            if (x >= width || y >= height || x < 0 || y < 0) {
                continue;
            }
            float size = 1.f;
            Vector2 pos = { (x-6) - player.pos.x + 1, (y-1.5) - player.pos.y + 1};

            bool isPlayer = false;
            if (player.pos.x == x && player.pos.y == y) {
                drawPlayer(player, pos, 0.5f);
                isPlayer = true;
            }
            drawEnemies(enemies, enemyArrSize, player, 0.5f, x, y);

            Vector2 matrixCoords = { x - player.pos.x + 4, y - player.pos.y + 4 };
            Vector2 mapPos = {x, y};

            drawTile(map[y][x], map, size, pos, matrixCoords, mapPos, isPlayer, player);
            map[y][x].col = (BoundingBox)
            {
                (Vector3) {pos.x - size/2, 0, pos.y - size/2},
                (Vector3) {pos.x + size/2, 0, pos.y + size/2},
            };
        }
    }
}

void drawSegmentInfo(Segment segment, Camera camera, Vector2 pos, Vector2 infoPos, float size) {
    Vector3 pos3D = {pos.x, pos.y, 0};
    drawSegment(segment, pos3D, size, 0, false, false, true);
    EndMode3D();

    int patternSize = 8;
    float xOffset = (patternSize*4) + (patternSize / 2);
    int center = (MeasureText("press 'space' to swap", 20) / 2) - xOffset;

    DrawText("press 'space' to swap", infoPos.x - center, infoPos.y - 25, 20, WHITE);
    drawPattern(segment.movePattern, infoPos.x, infoPos.y, patternSize);
    if (segment.combine) {
        DrawRing((Vector2){infoPos.x + xOffset, infoPos.y+100}, 5, 8, 0, 360, 20, ORANGE);
    }
    if (segment.moveType == 2) {
        DrawCircle((int)infoPos.x + xOffset, (int)infoPos.y+120, 8, BLUE);
    }
    else if (segment.moveType == 3) {
        DrawTexture(picMod, (int)infoPos.x + xOffset, (int)infoPos.y+120, WHITE);
    }
    else if (segment.moveType == 4) {
        DrawTexture(tntMod, (int)infoPos.x + xOffset, (int)infoPos.y+120, WHITE);
    }
    else {
        DrawCircle((int)infoPos.x + xOffset, (int)infoPos.y+120, 8, RED);
    }

    for (int i = 0; i < segment.movelength; i++) {
        DrawRectangle(infoPos.x +(patternSize*4)-(patternSize/2), infoPos.y + 140 + (i * 20), 16, 16, GREEN);
    }
}


void drawShopItemInfo(Tile tile, Camera camera, Vector2 pos, Vector2 uiPos) {
    Vector3 pos3D = {pos.x, pos.y, 0};
    if (tile.map) {
        rlPushMatrix();
            rlRotatef(rotation, pos3D.x, pos3D.y, pos3D.z);
            DrawModel(mapModel, pos3D, 1.f, WHITE);
        rlPopMatrix();
    }
    if (tile.pickaxe) {
        rlPushMatrix();
            rlRotatef(rotation, pos3D.x, pos3D.y, pos3D.z);
            DrawModel(pickaxeModel, pos3D, 1.f, WHITE);
            DrawModelWires(pickaxeModel, pos3D, 1.f, BLACK);
        rlPopMatrix();
    }
    if (tile.bombs) {
        rlPushMatrix();
            rlRotatef(rotation, pos3D.x, pos3D.y, pos3D.z);
            DrawModel(tntModel, pos3D, 1.f, WHITE);
            DrawModel(tntLabelModel, pos3D, 1.f, WHITE);
            DrawModelWires(tntModel, pos3D, 1.f, BLACK);
        rlPopMatrix();
    }
    EndMode3D();

    char* name;
    char* desc;
    unsigned short cost;

    if (tile.map) {
        name = "Mini Map";
        desc = "This will show a mini map on your screen";
        cost = 5000;
    }
    if (tile.pickaxe) {
        name = "Pickaxe Modifier";
        desc = "This will add a modifier that allows\n you to remove blocks.\nHowever you will not be able to move.";
        cost = 1000;
    }
    if (tile.bombs) {
        name = "Bomb Modifier";
        desc = "This will add a modifier that allows you\n to explode blocks in a radius.\nHowever you will not be able to move.";
        cost = 10000;
    }

    int fSize = 18;
    int nameOffset = MeasureText(name, 40) / 2;
    int costOffset = MeasureText(TextFormat("Costs: %i moneys", cost), fSize) / 2;
    int offset = MeasureText("Press Space to Purchase", fSize) / 2;

    DrawText(name, uiPos.x - nameOffset, uiPos.y, 40, GOLD);
    DrawText(desc, uiPos.x - 150, uiPos.y + 45, fSize, WHITE);
    DrawText("Press Space to Purchase", uiPos.x - offset, uiPos.y + 130, fSize, GREEN);
    DrawText(TextFormat("Costs: %i moneys", cost), uiPos.x - costOffset, uiPos.y + 130 + fSize, fSize, GREEN);
}


void drawSegment(Segment segment, Vector3 pos, float size, int sIdx, bool wires, bool selected, bool spinning) {
    Color color = segment.shape.color;
    Color wireColor = WHITE;
    int type = segment.shape.type;
    Vector3 pos3D = { pos.x, pos.y + (sIdx * size), pos.z};
    Vector3 pos3DCylinder = { pos.x, pos.y + (sIdx * size) - size/2, pos.z};

    if (wires) {
        if (selected) {
            wireColor = WHITE;
            wires = false;
        }
        else {
            wireColor = color;
        }
    }
    if (spinning) {
        rlPushMatrix();
        rlRotatef(rotation, pos3D.x, pos3D.y, pos3D.z);
    }
    switch (type) {
        case 0:
            if (!wires) DrawCube(pos3D, size, size, size, color);
            DrawCubeWires(pos3D, size, size, size, wireColor);
        break;

        case 1:
            if (!wires) DrawSphere(pos3D, size/2, color);
            DrawSphereWires(pos3D, size/2+0.03f, 8, 8, wireColor);
        break;

        case 2:
            if (!wires) DrawCylinder(pos3DCylinder, size/2, size/2, size, 8, color);
            DrawCylinderWires(pos3DCylinder, size/2, size/2, size, 8, wireColor);
        break;

        case 3:
            if (!wires) DrawCylinder(pos3DCylinder, size/2, 0.001f, size, 4, color);
            DrawCylinderWires(pos3DCylinder, size/2, 0.001f, size, 4, wireColor);
        break;

        case 4:
            if (!wires) DrawCylinder(pos3DCylinder, 0.001f, size/2, size, 4, color);
            DrawCylinderWires(pos3DCylinder, 0.001f, size/2, size, 4, wireColor);
        break;
    }
    if (spinning) {
        rlPopMatrix();
    }
}

void drawPlayerSegments(Player player, Vector3 pos, float size) {
    for (int sIdx = 0; sIdx < 3; sIdx++) {
        bool selected = (sIdx == player.selectedSegment);
        drawSegment(player.segments[sIdx], pos, size, sIdx, true, selected, true);
    }

}

void drawEnemyTiles(Enemy enemy, int size, Tile** map, float posX, float posY) {
    if (!enemy.dead) {
        for (int y = enemy.pos.y - 4; y < enemy.pos.y + 5; y++) {
            for (int x = enemy.pos.x - 4; x < enemy.pos.x + 5; x++) {
                Vector2 matrixCoords = { x - enemy.pos.x + 4, y - enemy.pos.y + 4 };
                Vector2 mapPos = {x, y};

                Color color;
                int type = enemy.movePattern[(int)matrixCoords.y][(int)matrixCoords.x];
                if (type == 1) {
                    color = RED;
                }
                else if (type == 2) {
                    color = SKYBLUE;
                }

                if (canAIMove(enemy, map, matrixCoords, mapPos)) {
                    float enX = x - enemy.pos.x;
                    float enY = y - enemy.pos.y;
                    Vector3 moveTilesPos = { posX + enX, -0.6, posY + enY };
                    rlPushMatrix();
                        rlRotatef(rotation, moveTilesPos.x, moveTilesPos.y, moveTilesPos.z);
                        DrawSphereWires(moveTilesPos, 0.25f, 6, 6, color);
                    rlPopMatrix();
                }
            }
        }
    }
}

void drawPattern(int pattern[9][9], int posX, int posY, int cellSize) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            Color color;
            if (pattern[y][x] == 1) {
                color = RED;
            }
            else if (pattern[y][x] == 2) {
                color = BLUE;
            }
            else {
                continue;
            }

            DrawRectangle(posX + (x * cellSize), posY + (y * cellSize), cellSize, cellSize, color);
        }
    }
}

void drawMiniMap(Tile** map, Enemy* enemies, int enemySize, Player player, int posX, int posY) {
        int size = 20;
        int cellSize = 4;

        for (int y = player.pos.y - size; y < player.pos.y + size + 1; y++) {
            for (int x = player.pos.x - size; x < player.pos.x + size + 1; x++) {
                if (x >= MAP_SIZE || y >= MAP_SIZE || x < 0 || y < 0) {
                    continue;
                }

                Color tileColor = WHITE;

                Vector2 matrixCoords = { x - player.pos.x + size, y - player.pos.y + size };
                int drawX = ((int) matrixCoords.x * cellSize) + posX;
                int drawY = ((int) matrixCoords.y * cellSize) + posY;

                Texture2D tileTex;

                switch (map[y][x].type) {
                    case FLOOR:
                        tileTex = floorTextures[map[y][x].texId];
                        tileColor = map[y][x].color;
                    break;
                    case PATH:
                        tileTex = pathwayTextures[map[y][x].texId];
                        tileColor = map[y][x].color;
                    break;
                    case WALL:
                        tileTex = wallTextures[map[y][x].texId];
                        tileColor = BLACK;
                    break;
                    default:
                        tileColor = WHITE;
                    break;
                }


                //DrawRectangle(drawX, drawY, 20, 20, tileColor);
                Rectangle source = {0,0,128,128};
                Rectangle dest = {drawX, drawY, cellSize, cellSize};
                DrawTexturePro(tileTex, source, dest, (Vector2){0,0}, 0, tileColor);

                if (x == player.pos.x && y == player.pos.y) {
                    DrawRectangle(drawX, drawY, cellSize, cellSize, GREEN);
                }

                if (map[y][x].goldblock) {
                    tileColor = GOLD;
                    tileColor.a = 100;
                    DrawRectangle(drawX, drawY, cellSize, cellSize, tileColor);
                }

                for (int i = 0; i < enemySize; i++) {
                    if (enemies[i].pos.x == x && enemies[i].pos.y == y) {
                        if (enemies[i].dead)
                            tileColor = SKYBLUE;
                        else
                            tileColor = RED;
                        DrawRectangle(drawX, drawY, cellSize, cellSize, tileColor);
                    }
                }

                if (map[y][x].type == PORTAL) {
                        DrawRectangle(drawX, drawY, cellSize, cellSize, PINK);
                }

            }
        }
}

void drawStats(Player player, int x, int y) {
    int fontSize = 30;
    DrawText(TextFormat("Level: %i", level), x, y, fontSize, WHITE);
    DrawText(TextFormat("Moneys: %i", player.moneys), x, y+fontSize, fontSize, WHITE);
    DrawText(TextFormat("Total Moves: %i", player.moves), x, y + (fontSize*2), fontSize, WHITE);
    DrawText(TextFormat("Total Kills: %i", player.kills), x, y + (fontSize*3), fontSize, WHITE);
}
