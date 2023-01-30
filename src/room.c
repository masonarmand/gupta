/*
 * rooms.c
 * Dungeon room generation
 */
#include "gupta.h"
#include <stdlib.h>

Room createRoom(Vector2 pos, int height, int width) {
    Room newRoom;

    newRoom.pos.y = pos.y;
    newRoom.pos.x = pos.x;
    newRoom.height = height;
    newRoom.width = width;
    newRoom.center.y = pos.y + (int)(height / 2);
    newRoom.center.x = pos.x + (int)(width / 2);

    return newRoom;
}

void addRoomToMap(Room room, Tile** map, bool isShop, bool spawnEnemy) {
    int maxHeight = room.pos.y + room.height;
    int maxWidth = room.pos.x + room.width;
    
    int r = GetRandomValue(1, LEVEL_COUNT - 1);
    if (isShop) r = 0;

    for (int y = room.pos.y; y <= maxHeight; y++) {
        for (int x = room.pos.x; x <= maxWidth; x++) {
            if (x >= MAP_SIZE || x < 0 || y >= MAP_SIZE || y < 0)
                continue;

            if (x == room.pos.x && y == room.pos.y && isShop) {
                map[y][x].type = PORTAL;
                continue;
            }
            if (x == room.center.x && y == room.center.y) {
                map[y][x].gold = GetRandomValue(0, 100);
                if (spawnEnemy) {
                    enemyArrSize = spawnNewEnemy(&enemies, enemyArrSize, x, y);
                }
            }
            if (map[y][x].type != PORTAL) {
                map[y][x].type = FLOOR;
                map[y][x].goldblock = false;
            }
            map[y][x].texId = r;
            
            if (isShop && ((x == room.pos.x || y  == room.pos.y) || (x == maxWidth || y == maxHeight))) {
                map[y][x].type = WALL;
                map[y][x].texId = 0;
            }

        }
    }
}

bool validPosition(int x, int y, Tile** map) {
    return ((x < MAP_SIZE && x > 0 || y < MAP_SIZE && y > 0) && map[y][x].type != FLOOR);
}

void connectRoomCenters(Vector2 centerOne, Vector2 centerTwo, Tile** map) {
    int x = (int)centerOne.x;
    int y = (int)centerOne.y;
    
    int x1 = (int)centerTwo.x;
    int y1 = (int)centerTwo.y;

    int dir;

    while (true) {
        if (abs((x - 1) - x1) < abs(x - x1)) {
            x--;
            dir = 0;
        }
        else if (abs((x + 1) - x1) < abs(x - x1)) {
            x++;
            dir = 1;
        }
        else if (abs((y + 1) - y1) < abs(y - y1)) {
            y++;
            dir = 2;
        }
        else if (abs((y - 1) - y1) < abs(y - y1)) {
            y--;
            dir = 3;
        }
        else break;

        if (validPosition(x,y,map)) {
            map[y][x].type = PATH;
            map[y][x].texId = GetRandomValue(0, PATH_COUNT - 1);
            map[y][x].goldblock = false;
        }
        
        // make hallways 3 wide
        switch (dir) {
            // horizontal hallways
            case 0:
            case 1:
                if (validPosition(x, y-1, map)) {
                    map[y-1][x].type = PATH;
                    map[y-1][x].texId = GetRandomValue(0, PATH_COUNT - 1);
                    map[y-1][x].goldblock = false;
                }

                if (validPosition(x, y+1, map)) {
                    map[y+1][x].type = PATH;
                    map[y+1][x].texId = GetRandomValue(0, PATH_COUNT - 1);
                    map[y+1][x].goldblock = false;
                }
            break;
            
            // vertical hallways
            case 2:
            case 3:
                if (validPosition(x-1, y, map)) {
                    map[y][x-1].type = PATH;
                    map[y][x-1].texId = GetRandomValue(0, PATH_COUNT - 1);
                    map[y][x-1].goldblock = false;
                }

                if (validPosition(x+1, y, map)) {
                    map[y][x+1].type = PATH;
                    map[y][x+1].texId = GetRandomValue(0, PATH_COUNT - 1);
                    map[y][x+1].goldblock = false;
                }
            break;
        }

    }
}
