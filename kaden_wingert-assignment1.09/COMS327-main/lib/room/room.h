#ifndef ROOM_H
#define ROOM_H

#define ROOM_WIDTH 80
#define ROOM_HEIGHT 21

#include "../tile/tile.h"
extern "C" {
#include "../../util/priorityqueue/priorityqueue.h"
}

typedef class Map Map;
typedef class Room Room;
typedef class Trainer Trainer;

class Room {
 public:
  int x, y;
  Tile tiles[ROOM_HEIGHT][ROOM_WIDTH];
  Tile *entrances[4];
  Trainer *trainers[ROOM_HEIGHT][ROOM_WIDTH];
  int curTime;
  PriorityQueue trainerQueue;
};

int createRoom(Map *map, Room *room, int x, int y);
int destroyRoom(Room *room);
int chooseRandomTile(Room *room, Tile **tile);
int populateRoom(Room *room);
int createPath(Map *map, Room *room);

#endif