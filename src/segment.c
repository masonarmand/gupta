/*
 * segment.c
 * Segment struct generation and functions
 */
#include "gupta.h"

int segmentKnight[9][9] =
{
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,2,0,2,0,0,0},
    {0,0,2,0,0,0,2,0,0},
    {0,0,0,0,9,0,0,0,0},
    {0,0,2,0,0,0,2,0,0},
    {0,0,0,2,0,2,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
};

int segmentCross[9][9] =
{
    {0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0},
    {1,1,1,1,9,1,1,1,1},
    {0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0},
};

int segmentX[9][9] =
{
    {1,0,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,0,1,0},
    {0,0,1,0,0,0,1,0,0},
    {0,0,0,1,0,1,0,0,0},
    {0,0,0,0,9,0,0,0,0},
    {0,0,0,1,0,1,0,0,0},
    {0,0,1,0,0,0,1,0,0},
    {0,1,0,0,0,0,0,1,0},
    {1,0,0,0,0,0,0,0,1},
};

bool isKnight(int pattern[9][9]) {
    bool isKnight = true;
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            if (segmentKnight[y][x] == 2 && pattern[y][x] == 0) {
                isKnight = false;
                goto breakloop;
            }
        }
    }
breakloop:
    return isKnight;
}

void copyPattern(Segment* segment, int pattern[9][9]) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            segment->movePattern[y][x] = pattern[y][x];
        }
    }
}


void generateSegment(Segment* segment) {
    int shapeId = GetRandomValue(0, 4);
    unsigned char r = GetRandomValue(0, 255);
    unsigned char g = GetRandomValue(0, 255);
    unsigned char b = GetRandomValue(0, 255);
    Color segmentColor = (Color) { r, g, b, 255 };
    segment->shape.type = shapeId;
    segment->shape.color = segmentColor;
    
    int moveType = GetRandomValue(0,2);
    int combineOdds = GetRandomValue(1, 100);
    int blueOdds = GetRandomValue(1, 100);

    segment->patternType = moveType;

    segment->combine = !(combineOdds <= 96);
    
    if (blueOdds <= 70) {
        segment->moveType = 1;
    }
    else {
        segment->moveType = 2;
    }

    segment->movelength = GetRandomValue(1, 4);

    switch (moveType) {
        case 0: copyPattern(segment, segmentKnight); break;
        case 1: copyPattern(segment, segmentCross); break;
        case 2: copyPattern(segment, segmentX); break;
    }
}
