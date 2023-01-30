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
#ifndef GUPTA_H
#define GUPTA_H

#include <raylib.h>

#define MIN(a, b) ((a)<(b)? (a) : (b))
#define MAP_SIZE 100
#define MAX_PORTALS 1
#define LEVEL_COUNT 5
#define PATH_COUNT 5

/*
 * Type definitions
 */

typedef enum { WALL, FLOOR, PATH, PORTAL, ALTAR} TileType;

typedef struct {
    unsigned char type;
    Color color;
} SegmentShape;

typedef struct {
    SegmentShape shape;
    int movePattern[9][9];
    unsigned char movelength;
    unsigned char moveType;
    unsigned char patternType;
    bool combine;
    bool pickaxe;
} Segment;

typedef struct {
    Vector2 pos;
    Segment segments[3];
    int movePattern[9][9];
    bool dead;
    bool aggro;
} Enemy;

typedef struct {
    Vector2 pos;
    Segment segments[3];
    unsigned char selectedSegment;
    unsigned int moneys;
    int movePattern[9][9];
    bool dead;
    bool hasMap;
    Enemy killer;

    // stats
    unsigned short kills;
    unsigned int moves;
} Player;

typedef struct {
    TileType type;
    unsigned char texId;

    Color color;
    Color selectionColor;
    BoundingBox col;
    Segment segment;
    bool loot;
    bool map;
    bool pickaxe;
    bool bombs;
    bool goldblock;
    unsigned short worth;
    unsigned short gold;
} Tile;

typedef struct {
    int height;
    int width;
    Vector2 pos;
    Vector2 center;
} Room;


typedef struct {
  double startTime;
  double lifeTime;
} Timer;

/*
 * Externs
 */
extern Texture2D tiletex;
extern Texture2D tiletexalt;
extern Texture2D tileMarkerTex;
extern Texture2D tileJumpTex;
extern Texture2D bombTex;
extern Texture2D playerTileTex;
extern Texture2D pickaxeTileTex;
extern Texture2D goldTex;
extern Texture2D tntMod;
extern Texture2D picMod;
extern Texture2D floorTextures[LEVEL_COUNT];
extern Texture2D wallTextures[LEVEL_COUNT];
extern Texture2D pathwayTextures[PATH_COUNT];
extern int rotation;
extern int level;
extern char debugText[50];
extern int portalCount;
extern float hoverPosX;
extern float hoverPosY;
extern bool drawAITiles;
extern bool playersTurn;
extern bool debug;

extern Sound moveSnd;
extern Sound selectSnd;
extern Sound switchSnd;
extern Sound goldSnd;
extern Sound enemyDeathSnd;
extern Sound playerDeathSnd;
extern Sound bombSnd;

extern Model mapModel;
extern Model pickaxeModel;
extern Model tntModel;
extern Model tntLabelModel;
extern Model altarModel;

extern int enemyArrSize;
extern Enemy* enemies;
extern Enemy enemyHover;
extern Timer moveTimer;

void updateLevel(void);  // Drawing/Rendering the game

/*
 * maths.c functions
 */
bool bresenham(int x1, int y1, int x2, int y2, int patternPosY, Tile** map, bool vertical);
void removeCircle(int startX, int startY, int radius, Tile** map);

/*
 *timer.c functions
 */
void startTimer(Timer *timer, double lifetime);
void resetTimer(Timer *timer);
bool TimerDone(Timer timer);
double GetElapsed(Timer timer);


/*
 * engine.c prototyped functions
 */
Tile** initMap(int width, int height);
void resetMap(Tile** map, int width, int height);
void unloadMap(Tile** map, int height);
void setWallTexture(Tile** map, int id);
Vector2 generateMap(Tile** map);
Vector2 generateShop(Tile** map);
bool canMove(Player player, Tile** map, Vector2 patternPos, Vector2 mapPos);
void processHover(Ray ray, Tile** map, Player* player);
bool isFloor(TileType type);

/*
 * draw.c prototyed functions
 */
void drawPlayerSegments(Player player, Vector3 pos, float size);
void drawMapArea(Player player, Tile** map, int width, int height);
void drawPattern(int pattern[9][9], int posX, int posY, int cellSize);
void drawSegmentInfo(Segment segment, Camera camera, Vector2 pos, Vector2 infoPos, float size);
void drawShopItemInfo(Tile tile, Camera camera, Vector2 pos, Vector2 uiPos);
void drawMiniMap(Tile** map, Enemy* enemies, int enemySize, Player player, int posX, int posY);
void drawStats(Player player, int x, int y);


/*
 * room.c prototyped functions
 */
Room createRoom(Vector2 pos, int height, int width);
void addRoomToMap(Room room, Tile** map, bool isShop, bool spawnEnemy);
void connectRoomCenters(Vector2 centerOne, Vector2 centerTwo, Tile** map);

/*
 * segment.c prototyped functions
 */
void generateSegment(Segment* segment);
bool isKnight(int pattern[9][9]);

/*
 * player.c prototyped functions
 */
void generatePlayer(Player* player);
void updateMovePattern(Player* player);
void switchSegment(Player* player, int dir);


/*
 * enemy.c prototyped functions
 */
int spawnNewEnemy(Enemy** enemies, int size, int x, int y);
void updateEnemyPositions(Enemy* enemies, int size, Player* player, Tile** map, int posX, int posY);
void killDeadEnemies(Enemy* enemies, int size, Tile** map, Player* player);
bool canAIMove(Enemy enemy, Tile** map, Vector2 patternPos, Vector2 mapPos);
bool isEnemyInView(Enemy* enemy, Player player, Tile** map);
void drawEnemyTiles(Enemy enemy, int size, Tile** map, float posX, float posY);
#endif
