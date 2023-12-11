#ifndef TILE_H
#define TILE_H

#define NUM_SPECIAL_TILES 1
enum SpecialTile { EMPTY = 0 };

#define NUM_REGION_TILES 6
enum RegionTile { GRASS = 1, GRASS_2, TALL_GRASS, TALL_GRASS_2, FOREST, WATER };

enum EnvironmentTile { BOULDER = 7, PATH, ENTRANCE };

enum BuildingTile {
  POKEMON_CENTER = 10,
  POKEMART,
};

class Tile {
 public:
  int type;
  int x, y;
};

int createTile(Tile *tile, int type, int x, int y);
int destroyTile(Tile *tile);
char getTileCharacter(Tile tile);
double getTileDistance(Tile tile1, Tile tile2);
double getDistance(int x1, int y1, int x2, int y2);

#endif