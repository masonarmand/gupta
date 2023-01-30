/*
 * maths.c
 * Algorithms and math related functions
 */
#include "gupta.h"
#include <stdlib.h>
    
bool bresenham(int x1, int y1, int x2, int y2, int patternPosY, Tile** map, bool vertical) {
    int x3 = x1;
    int y3 = y1;
    int x4 = x2;
    int y4 = y2;

    // Bresenham's Line Algorithm
    bool canMove = true;
    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if (map[y2][x2].type == WALL) {
        canMove = false;
    }

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
    
    if (vertical && patternPosY != 4) {
        // check vertical lines
        if (patternPosY > 4) {
            for (int y = y3; y <= y4; y++) {
                if (map[y][x3].type == WALL) {
                    canMove = false;
                    break;
                }
            }
        }
        
        if (patternPosY < 4) {
            for (int y = y3; y >= y4; y--) {
                if (map[y][x3].type == WALL) {
                    canMove = false;
                    break;
                }
            }
        }
    }

    return canMove;
}


void removeCircle(int startX, int startY, int radius, Tile** map) {
    for (int y = startY - radius; y <= startY + radius; y++) {
        for (int x = startX - radius; x <= startX + radius; x++) {
            if (x >= MAP_SIZE || y >= MAP_SIZE || x < 0 || y < 0) {
                continue;
            }

            if ((x-startX)*(x-startX) + (y-startY)*(y-startY) <= radius*radius) {
                if (map[y][x].type != PORTAL && !map[y][x].goldblock) {
                    map[y][x].type = FLOOR;
                    map[y][x].loot = false;
                    unsigned char val = GetRandomValue(20, 120);
                    Color color = {val, val, val, 255};
                    map[y][x].color = color;
                }
            }
        }
    }
}
