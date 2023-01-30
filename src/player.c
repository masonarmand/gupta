/*
 * player.c
 * Player movement, generation, and movestyle generation
 */
#include "gupta.h"

void generatePlayer(Player* player) {
    for (int i = 0; i < 3; i++) {
        generateSegment(&player->segments[i]);
    }
    updateMovePattern(player);
}

void copyPlayerPattern(Player* player, int pattern[9][9]) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            if (pattern[y][x] != 0) {
                player->movePattern[y][x] = pattern[y][x];
            }
        }
    }
}

void resetPlayerPattern(Player* player) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            player->movePattern[y][x] = 0;
        }
    } 
}

void switchSegment(Player* player, int dir) {
    int selected = player->selectedSegment;
    int idx = selected + dir;

    if (idx >= 0 && idx <= 2) {
        Segment tempSeg = player->segments[selected];
        player->segments[selected] = player->segments[idx];
        player->segments[idx] = tempSeg;
        updateMovePattern(player);

        PlaySound(switchSnd);
    }
}

void updateMovePattern(Player* player) {
    resetPlayerPattern(player);
    int moveType = player->segments[2].moveType;
    int movelength = player->segments[1].movelength;
    bool combine = player->segments[2].combine;

    copyPlayerPattern(player, player->segments[0].movePattern);
    if (combine) {
        copyPlayerPattern(player, player->segments[1].movePattern);
    }
    
    int crop = -1;
    switch(movelength) {
        case 1: crop = 2; break;
        case 2: crop = 1; break;
        case 3: crop = 0; break;
    }

    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            int type = player->movePattern[y][x];
            if (type != 0 && moveType != 1) {
                player->movePattern[y][x] = moveType;
            }
            if (crop != -1 && !isKnight(player->movePattern) && ((y <= crop || x <= crop) || (y >= 8 - crop || x >= 8 - crop))) {
                player->movePattern[y][x] = 0; 
            }
        }
    }
    
    
}
