/*
 * enemy.c
 * Enemy AI, spawning, etc
 */
#include "gupta.h"
#include <math.h>
#include <stdio.h>
#include <rlgl.h>
#include <stdlib.h>

void updateEnemyMovePattern(Enemy* enemy);
void resetEnemyPattern(Enemy* enemy);
bool canAIMove(Enemy enemy, Tile** map, Vector2 patternPos, Vector2 mapPos);
void copyEnemyPattern(Enemy* enemy, int pattern[9][9]);

void generateEnemy(Enemy* enemy) {
    for (int i = 0; i < 3; i++) {
        generateSegment(&enemy->segments[i]);
    }
    updateEnemyMovePattern(enemy);
    enemy->dead = false;
    enemy->aggro = false;
}

int spawnNewEnemy(Enemy** enemies, int size, int x, int y) {
    int newSize = size + 1;
    Enemy* newAddr = (Enemy*) realloc(*enemies, (newSize * sizeof(Enemy)));

    if (newAddr == NULL) {
        printf("ERROR: Unable to realloc memory for enemies array\n");
        return 0;
    }
    else {
        newAddr[newSize - 1].pos.x = x;
        newAddr[newSize - 1].pos.y = y;

        generateEnemy(&newAddr[newSize - 1]);
        *enemies = newAddr;
    }

    return newSize;
}

bool canAIMove(Enemy enemy, Tile** map, Vector2 patternPos, Vector2 mapPos) {
    if (enemy.movePattern[(int)patternPos.y][(int)patternPos.x] == 0) {
        return false;
    }
    bool vertical = (enemy.segments[0].moveType == 1);
    return bresenham(enemy.pos.x, enemy.pos.y, mapPos.x, mapPos.y, patternPos.y, map, vertical);
}

void copyEnemyPattern(Enemy* enemy, int pattern[9][9]) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            if (pattern[y][x] != 0) {
                enemy->movePattern[y][x] = pattern[y][x];
            }
        }
    }
}

void resetEnemyPattern(Enemy* enemy) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            enemy->movePattern[y][x] = 0;
        }
    } 
}

void updateEnemyMovePattern(Enemy* enemy) {
    resetEnemyPattern(enemy);
    int moveType = enemy->segments[0].moveType;
    int movelength = enemy->segments[1].movelength;
    bool combine = enemy->segments[2].combine;
    copyEnemyPattern(enemy, enemy->segments[0].movePattern);

    if (combine) {
        copyEnemyPattern(enemy, enemy->segments[1].movePattern);
    }
    
    int crop = -1;
    switch(movelength) {
        case 1: crop = 2; break;
        case 2: crop = 1; break;
        case 3: crop = 0; break;
    }

    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            int type = enemy->movePattern[y][x];
            if (type == 1) {
                enemy->movePattern[y][x] = moveType;
            }
            if (crop != -1 && !isKnight(enemy->movePattern) && ((y <= crop || x <= crop) || (y >= 8 - crop || x >= 8 - crop))) {
                enemy->movePattern[y][x] = 0; 
            }
        }
    }
}

bool isEnemyInView(Enemy* enemy, Player player, Tile** map) {
    bool isInView = false;

    for (int y = player.pos.y - 4; y < player.pos.y + 5; y++) {
        for (int x = player.pos.x - 4; x < player.pos.x + 5; x++) {
            if (enemy->pos.x == x && enemy->pos.y == y) {
                isInView = true;
            }
        }
    }
    if (enemy->aggro && !isInView) {
        enemy->aggro = false;
    }

    return isInView;
}


void dropEnemyLoot(Enemy enemy, Tile** map) {
    int x = (int) enemy.pos.x;
    int y = (int) enemy.pos.y;
    
    map[y][x].segment = enemy.segments[0];
    map[y][x].loot = true;
}

void killDeadEnemies(Enemy* enemies, int size, Tile** map, Player* player) {
    if (playersTurn && !player->dead) {
        for (int i = 0; i < size; i++) {
            if (!enemies[i].dead && enemies[i].pos.x == player->pos.x &&
                                        enemies[i].pos.y == player->pos.y) 
            {
                enemies[i].dead = true;
                dropEnemyLoot(enemies[i], map);
                PlaySound(enemyDeathSnd);
                player->kills += 1;
            }
        }
    }
}

void updateEnemyPositions(Enemy* enemies, int size, Player* player, Tile** map, int posX, int posY) {
    playersTurn = false;
    for (int i = 0; i < size; i++) { 
        if (isEnemyInView(&enemies[i], *player, map) && !enemies[i].dead) {
            if (!enemies[i].aggro) {
                enemies[i].aggro = true;
                break;
            }
            Vector2 movePos = enemies[i].pos;
            Vector2 randPos = enemies[i].pos;
            Vector2 closestPos = enemies[i].pos;
            Vector2 advMove = enemies[i].pos;

            for (int y = player->pos.y - 4; y < player->pos.y + 5; y++) {
                for (int x = player->pos.x - 4; x < player->pos.x + 5; x++) {
                    // Calculate closest position to player
                    Vector2 matrixCoords = { x - enemies[i].pos.x + 4, y - enemies[i].pos.y + 4 };
                    Vector2 mapPos = {x, y};
                    
                    if (matrixCoords.x > 8 || matrixCoords.x < 0 || 
                        matrixCoords.y > 8 || matrixCoords.y < 0 ||
                        x >= MAP_SIZE || x < 0 || y >= MAP_SIZE || y < 0)
                    {
                        continue;
                    }

                    int x1 = mapPos.x;
                    int y1 = mapPos.y;
                    
                    int lastx1 = closestPos.x;
                    int lasty1 = closestPos.y;
                    
                    int x2 = player->pos.x;
                    int y2 = player->pos.y;

                    double distance = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
                    double distanceLast = sqrt(pow((lastx1 - x2), 2) + pow((lasty1 - y2), 2));

                    if (canAIMove(enemies[i], map, matrixCoords, mapPos) &&
                        distance < distanceLast)
                    {
                        closestPos = mapPos;
                    }
                    
                    bool vertical = (enemies[i].segments[0].moveType == 1);
                    // Advanced move
                    if (canAIMove(enemies[i], map, matrixCoords, mapPos))
                    {
                        Vector2 enemyTempPos = enemies[i].pos;
                        enemies[i].pos = mapPos; // temporary, used to check if
                                                 // can kill player
                        Vector2 matrixCoordsPlayer = {player->pos.x - enemies[i].pos.x + 4, player->pos.y - enemies[i].pos.y + 4 };
                        if (canAIMove(enemies[i], map, matrixCoordsPlayer, mapPos)) {
                            advMove = mapPos;
                        }
                        enemies[i].pos = enemyTempPos;
                    }

                    // Random move
                    if (GetRandomValue(1, 100) <= 25 && canAIMove(enemies[i], map, matrixCoords, mapPos)) {
                        randPos = mapPos;
                    }
                    
                }
            }
            int odds = GetRandomValue(1, 3);

            if (odds == 1)
                movePos = closestPos;
            else if (odds == 2)
                movePos = randPos;
            else if (odds == 3)
                movePos = advMove;

            if (odds != 1 && closestPos.x == player->pos.x && closestPos.y == player->pos.y) {
                // 5 percent chance for enemy to make mistake
                if (GetRandomValue(1, 100) <= 95)
                    movePos = closestPos;
            }
            
            enemies[i].pos = movePos;

            if (enemies[i].pos.x == player->pos.x &&
                enemies[i].pos.y == player->pos.y) 
            {
                player->dead = true;
                PlaySound(playerDeathSnd);
                player->killer = enemies[i];
            }
        }
    }
    playersTurn = true;
}
