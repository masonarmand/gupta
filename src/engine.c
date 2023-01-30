/*
 * engine.c
 * Internal functions to handle game input,
 * map generation, and more
 */
#include <math.h>
#include <stdlib.h> // required for malloc(), free()
#include "gupta.h"
#include <rlgl.h>
#include <stdio.h>

void generateAltarRoom(Tile** map);

Tile** initMap(int width, int height) {
    Tile** map = calloc(height, sizeof(Tile*));

    for (int y = 0; y < height; y++) {
        map[y] = calloc(width, sizeof(Tile));
        for (int x = 0; x < width; x++) {
            map[y][x].type = WALL;
            map[y][x].texId = 4;
            map[y][x].gold = 0;
            map[y][x].loot = false;
            map[y][x].map = false;
            if (GetRandomValue(1, 100) == 1) {
                map[y][x].goldblock = true;
                map[y][x].worth = GetRandomValue(100, 500);
            }

        }
    }

    return map;
}

void resetMap(Tile** map, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            map[y][x].type = WALL;
            map[y][x].texId = 4;
            map[y][x].gold = 0;
            map[y][x].goldblock = false;
            map[y][x].worth = 0;
            map[y][x].loot = false;
            map[y][x].map = false;
            map[y][x].bombs = false;
            map[y][x].pickaxe = false;
            if (GetRandomValue(1, 100) == 1) {
                map[y][x].goldblock = true;
                map[y][x].worth = GetRandomValue(100, 500);
            }
        }
    }
}

void updateTextures(Tile** map) {
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            map[y][x].color = WHITE;
            map[y][x].selectionColor = WHITE;
            int noise = GetRandomValue(1,3);
            int col = GetRandomValue(150, 255);
            if (noise == 1) {
                map[y][x].color = (Color){col,col,col, 255};
            }
        }
    }
}

void setWallTexture(Tile** map, int id) {
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            map[y][x].texId = id;
        }
    }
}

void unloadMap(Tile** map, int height) {
    for (int y = 0; y < height; y++) {
        free(map[y]);
    }
    free(map);
}

bool roomOverlaps(Room* rooms, int rooms_counter, int y, int x, int height, int width) {
    for (int i = 0; i < rooms_counter; i++) {
        if (x >= rooms[i].pos.x + rooms[i].width || rooms[i].pos.x >= x + width) {
            continue;
        }
        if (y + height <= rooms[i].pos.y || rooms[i].pos.y + rooms[i].height <= y) {
            continue;
        }

        return true;
    }

    return false;
}

Vector2 generateMap(Tile** map) {
    Vector2 pos;
    Vector2 startPos;
    int height, width, numRooms;
    numRooms = GetRandomValue(10, 20);
    Room* rooms = calloc(numRooms, sizeof(Room));

    int rooms_counter = 0;

    if (GetRandomValue(1, 100) <= 10) {
        generateAltarRoom(map);
    }

    for (int i = 0; i < numRooms; i++) {
        pos.y = GetRandomValue(20, MAP_SIZE - 20);
        pos.x = GetRandomValue(20, MAP_SIZE - 20);
        height = GetRandomValue(7, 10);
        width = GetRandomValue(8, 15);

        int shopOdds = GetRandomValue(1, 100);

        if (!roomOverlaps(rooms, rooms_counter, (int)pos.y, (int)pos.x, height, width)) {
            rooms[rooms_counter] = createRoom(pos, height, width);
            bool isShop = (shopOdds <= 10);
            bool spawnEnemy = (rooms_counter != 0 && !isShop);
            addRoomToMap(rooms[rooms_counter], map, isShop, spawnEnemy);
            rooms_counter++;
        }
    }

    for (int i = 1; i < rooms_counter; i++) {
        connectRoomCenters(rooms[i-1].center, rooms[i].center, map);
    }
    int portalX = rooms[rooms_counter - 1].pos.x;
    int portalY = rooms[rooms_counter - 1].pos.y;

    map[portalY][portalX].type = PORTAL;

    startPos.y = rooms[0].center.y;
    startPos.x = rooms[0].center.x;

    free(rooms);

    updateTextures(map);

    return startPos;
}

void generateAltarRoom(Tile** map) {
    int x = GetRandomValue(10, MAP_SIZE - 10);
    int y = 4;
    int r = GetRandomValue(5, 8);

    removeCircle(x, y, r, map);

    map[y][x].type = ALTAR;
}

Vector2 generateShop(Tile** map) {
    Vector2 startPos;
    Vector2 pos;

    int height, width;

    pos.y = GetRandomValue(20, MAP_SIZE - 20);
    pos.x = GetRandomValue(20, MAP_SIZE - 20);
    height = GetRandomValue(7, 10);
    width = GetRandomValue(8, 15);
    Room room = createRoom(pos, height, width);
    addRoomToMap(room, map, true, false);
    startPos = room.center;
    updateTextures(map);

    map[(int) pos.y + 1][(int) startPos.x].map = true;
    map[(int) pos.y + 1][(int) startPos.x - 1].pickaxe = true;
    map[(int) pos.y + 1][(int) startPos.x + 1].bombs = true;

    return startPos;
}

bool canMove(Player player, Tile** map, Vector2 patternPos, Vector2 mapPos) {
    bool canMove = true;
    int x1 = (int) player.pos.x;
    int y1 = (int) player.pos.y;
    int x2 = (int) mapPos.x;
    int y2 = (int) mapPos.y;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    // Bresenham's Line Algorithm
    int pk = 2 * dy - dx;
    for (int i = 0; i <= dx; i++) {
        if (map[y1][x1].type == WALL) {
            canMove = false;
            break;
        }

        x1 < x2 ? x1++ : x1--;
        if (pk < 0) {
            pk = pk + 2 * dy;
        }
        else {
            y1 < y2 ? y1++ : y1--;
            pk = pk + 2 * dy - 2 * dx;
        }
    }

    // check vertical lines
    if (player.segments[0].patternType == 1 && patternPos.y != 4) {
        x1 = (int) player.pos.x;
        y1 = (int) player.pos.y;
        x2 = (int) mapPos.x;
        y2 = (int) mapPos.y;

        if (patternPos.y > 4) {
            for (int y = y1; y <= y2; y++) {
                if (map[y][x1].type == WALL) {
                    canMove = false;
                    break;
                }
            }
        }

        if (patternPos.y < 4) {
            for (int y = y1; y >= y2; y--) {
                if (map[y][x1].type == WALL) {
                    canMove = false;
                    break;
                }
            }
        }

    }

    return canMove;
}

void movePlayer(Player* player, Tile** map, int x, int y) {
    player->pos.y = y;
    player->pos.x = x;
    killDeadEnemies(enemies, enemyArrSize, map, player);
    updateEnemyPositions(enemies, enemyArrSize, player, map, x, y);
    player->moves += 1;
}

void enableEnemyMoveset(Player player, int x, int y) {
    for (int i = 0; i < enemyArrSize; i++) {
        if (enemies[i].pos.x == x && enemies[i].pos.y == y) {
            hoverPosX = (x-6) - player.pos.x + 1;
            hoverPosY = (y-1.5) - player.pos.y + 1;
            enemyHover = enemies[i];
            drawAITiles = true;
            break;
        }
        else {
            drawAITiles = false;
        }
    }
}

void processInput(Player* player, Tile** map, int x, int y) {
    if (IsMouseButtonPressed(0) && TimerDone(moveTimer)) {
        int posX = x - player->pos.x + 4;
        int posY = y - player->pos.y + 4;
        int patternTile = player->movePattern[posY][posX];

        if (patternTile != 0 && map[y][x].type != WALL) {
            PlaySound(moveSnd);

            Vector2 patternPos = {posX, posY};
            Vector2 mapPos = {x,y};

            if ((patternTile == 1 &&
                 canMove(*player, map, patternPos, mapPos)) ||
                 patternTile == 2)
            {
                movePlayer(player, map, x, y);
            }
            if (map[y][x].gold > 0) {
                PlaySound(goldSnd);
                player->moneys += map[y][x].gold;
                map[y][x].gold = 0;
            }
        }

        if (patternTile == 3) {
            map[y][x].type = FLOOR;
            if (map[y][x].goldblock) {
                player->moneys += map[y][x].worth;
                PlaySound(goldSnd);
                map[y][x].goldblock = false;
                map[y][x].worth = 0;
            }
        }

        if (patternTile == 4) {
            removeCircle(x, y, GetRandomValue(2, 6), map);
            PlaySound(bombSnd);
        }
        startTimer(&moveTimer, 0.01f);
    }
}

void processHover(Ray ray, Tile** map, Player* player) {
    for (int y = player->pos.y - 4; y < player->pos.y + 5; y++) {
        for (int x = player->pos.x - 4; x < player->pos.x + 5; x++) {

            if (x >= MAP_SIZE || y >= MAP_SIZE || x < 0 || y < 0)
                continue;

            RayCollision collision = GetRayCollisionBox(ray, map[y][x].col);
            if (collision.hit) {
                enableEnemyMoveset(*player, x, y);

                if (map[y][x].type == PORTAL) {
                    map[y][x].selectionColor = VIOLET;
                    if (IsMouseButtonPressed(0)) {
                        level += 1;
                        updateLevel();
                    }
                }
                else {
                    map[y][x].selectionColor = RED;
                }
                processInput(player, map, x, y);
            }
            else {
                map[y][x].selectionColor = WHITE;
            }

        }
    }
}

bool isFloor(TileType type) {
    return (type == FLOOR || type == PORTAL || type == PATH);
}
