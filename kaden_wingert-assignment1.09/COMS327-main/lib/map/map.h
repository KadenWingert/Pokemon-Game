#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 401
#define MAP_HEIGHT 401

#define N 0
#define E 1
#define S 2
#define W 3

typedef class Room Room;

class Map {
 public:
  int x, y;
  Room *rooms[MAP_HEIGHT][MAP_WIDTH];
  int trainersPerRoom;
};

int createMap(Map *map, int trainersPerRoom);
int destroyMap(Map *map);

#endif