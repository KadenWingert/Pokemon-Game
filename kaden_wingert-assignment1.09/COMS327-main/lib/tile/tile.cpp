#include "tile.h"

#include <cmath>

int createTile(Tile *tile, int type, int x, int y) {
  tile->type = type;
  tile->x = x;
  tile->y = y;

  return 0;
}

int destroyTile(Tile *tile) {
  tile->type = EMPTY;
  tile->x = -1;
  tile->y = -1;

  return 0;
}

char getTileCharacter(Tile tile) {
  switch (tile.type) {
    case EMPTY:
      break;       // EMPTY
    case GRASS:    // GRASS
    case GRASS_2:  // GRASS_2
      return '.';
    case TALL_GRASS:    // TALL_GRASS
    case TALL_GRASS_2:  // TALL_GRASS_2
      return ':';
    case FOREST:  // FOREST
      return '^';
    case WATER:  // WATER
      return '~';
    case BOULDER:  // BOULDER
      return '%';
    case PATH:  // PATH
      return '#';
    case ENTRANCE:  // GATE
      return '#';
    case POKEMON_CENTER:  // POKEMON_CENTER
      return 'C';
    case POKEMART:  // POKEMART
      return 'M';
  }

  return '?';  // EMPTY
}

double getTileDistance(Tile tile1, Tile tile2) {
  return sqrt(pow(tile1.x - tile2.x, 2) + pow(tile1.y - tile2.y, 2));
}

double getDistance(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}