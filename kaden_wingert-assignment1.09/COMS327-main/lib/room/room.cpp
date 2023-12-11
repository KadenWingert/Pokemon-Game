#include "room.h"

#include <cstdlib>

extern "C" {
#include "../../util/queue/queue.h"
}
#include "../map/map.h"
#include "../room/room.h"
#include "../trainer/trainer.h"

int createRoom(Map *map, Room *room, int i, int j) {
  room->x = i;
  room->y = j;

  room->entrances[N] = NULL;
  room->entrances[E] = NULL;
  room->entrances[S] = NULL;
  room->entrances[W] = NULL;

  room->curTime = 0;

  int y, x;

  for (y = 0; y < ROOM_HEIGHT; y++) {
    for (x = 0; x < ROOM_WIDTH; x++) {
      createTile(&room->tiles[y][x], EMPTY, x, y);
      room->trainers[y][x] = NULL;
    }
  }

  populateRoom(room);
  createPath(map, room);

  for (int i = 0; i < map->trainersPerRoom; i++) {
    Trainer *trainer = new Trainer();

    if (i <= 1) {
      trainer->type = (TrainerType)i;
    }

    placeTrainerInRoom(room->trainers, trainer, room);
  }

  return 0;
}

int destroyRoom(Room *room) {
  room->x = -1;
  room->y = -1;

  room->entrances[N] = NULL;
  room->entrances[E] = NULL;
  room->entrances[S] = NULL;
  room->entrances[W] = NULL;

  int y, x;

  for (y = 0; y < ROOM_HEIGHT; y++) {
    for (x = 0; x < ROOM_WIDTH; x++) {
      destroyTile(&room->tiles[y][x]);

      if (room->trainers[y][x] != NULL) {
        destroyTrainer(room->trainers[y][x]);
      }
    }
  }

  free(room);

  return 0;
}

int chooseRandomTile(Room *room, Tile **tile) {
  int y, x;

  y = rand() % ROOM_HEIGHT;
  x = rand() % ROOM_WIDTH;

  *tile = &room->tiles[y][x];

  return 0;
}

int populateRoom(Room *room) {
  Tile *tile[NUM_REGION_TILES];

  Queue regionQueue;
  createQueue(&regionQueue);

  int i;

  for (i = 0; i < NUM_REGION_TILES; i++) {
    int hasDistance = 0;

    while (!hasDistance) {
      chooseRandomTile(room, &tile[i]);

      if (tile[i]->type != EMPTY) {
        continue;
      }

      hasDistance = 1;

      int j;

      for (j = 0; j < i; j++) {
        if (getTileDistance(*tile[i], *tile[j]) < 12) {
          hasDistance = 0;
          break;
        }
      }

      if (!hasDistance) {
        continue;
      }

      tile[i]->type = i + NUM_SPECIAL_TILES;

      enqueue(&regionQueue, tile[i]);
    }
  }

  while (regionQueue.size > 0) {
    Tile *currentTile;
    dequeue(&regionQueue, (void **)&currentTile);

    int y, x;

    for (y = currentTile->y - 1; y <= currentTile->y + 1; y++) {
      for (x = currentTile->x - 1; x <= currentTile->x + 1; x++) {
        if ((y == currentTile->y && x == currentTile->x) || y < 0 ||
            y >= ROOM_HEIGHT || x < 0 || x >= ROOM_WIDTH) {
          continue;
        }

        Tile *adjacentTile = &room->tiles[y][x];

        if (adjacentTile->type != EMPTY) {
          continue;
        }

        adjacentTile->type = currentTile->type;

        enqueue(&regionQueue, adjacentTile);
      }
    }
  }

  for (i = 0; i < ROOM_WIDTH; i++) {
    room->tiles[0][i].type = BOULDER;
    room->tiles[ROOM_HEIGHT - 1][i].type = BOULDER;
  }

  for (i = 0; i < ROOM_HEIGHT; i++) {
    room->tiles[i][0].type = BOULDER;
    room->tiles[i][ROOM_WIDTH - 1].type = BOULDER;
  }

  int numBoulders = rand() % 5 + 8;

  for (i = 0; i < numBoulders; i++) {
    Tile *currentTile;

    chooseRandomTile(room, &currentTile);

    if (currentTile->type == WATER) {
      continue;
    }

    currentTile->type = BOULDER;
  }

  return 0;
}

int createPath(Map *map, Room *room) {
  int pathAllowed[4] = {room->y > 0, room->x != MAP_WIDTH - 1,
                        room->y != MAP_HEIGHT - 1, room->x > 0};  // N, E, S, W

  int i;
  Tile *adjacentEntrance;
  Room *adjacentRoom;

  if (pathAllowed[W]) {
    adjacentRoom = map->rooms[room->y][room->x - 1];

    if (adjacentRoom != NULL) {
      adjacentEntrance = adjacentRoom->entrances[E];
      room->entrances[W] = &room->tiles[adjacentEntrance->y][0];
    }
  }

  if (room->entrances[W] == NULL) {
    i = rand() % (ROOM_HEIGHT - 4) + 2;

    room->entrances[W] = &room->tiles[i][0];
  }

  if (pathAllowed[E]) {
    adjacentRoom = map->rooms[room->y][room->x + 1];

    if (adjacentRoom != NULL) {
      adjacentEntrance = adjacentRoom->entrances[W];
      room->entrances[E] = &room->tiles[adjacentEntrance->y][ROOM_WIDTH - 1];
    }
  }

  if (room->entrances[E] == NULL) {
    i = rand() % (ROOM_HEIGHT - 4) + 2;

    room->entrances[E] = &room->tiles[i][ROOM_WIDTH - 1];
  }

  if (pathAllowed[N]) {
    adjacentRoom = map->rooms[room->y - 1][room->x];

    if (adjacentRoom != NULL) {
      adjacentEntrance = adjacentRoom->entrances[S];
      room->entrances[N] = &room->tiles[0][adjacentEntrance->x];
    }
  }

  if (room->entrances[N] == NULL) {
    i = rand() % (ROOM_WIDTH - 4) + 2;

    room->entrances[N] = &room->tiles[0][i];
  }

  if (pathAllowed[S]) {
    adjacentRoom = map->rooms[room->y + 1][room->x];

    if (adjacentRoom != NULL) {
      adjacentEntrance = adjacentRoom->entrances[N];
      room->entrances[S] = &room->tiles[ROOM_HEIGHT - 1][adjacentEntrance->x];
    }
  }

  if (room->entrances[S] == NULL) {
    i = rand() % (ROOM_WIDTH - 4) + 2;

    room->entrances[S] = &room->tiles[ROOM_HEIGHT - 1][i];
  }

  double distanceFromCenter =
      getDistance(room->x, room->y, MAP_WIDTH / 2, MAP_HEIGHT / 2);

  int shouldHavePokemonCenter =
      rand() % 100 < ((-45 * distanceFromCenter) / 200 + 50);

  if (distanceFromCenter >= 200) {
    shouldHavePokemonCenter = rand() % 20 == 0;
  }

  int shouldHavePokemart =
      rand() % 100 < ((-45 * distanceFromCenter) / 200 + 50);

  if (distanceFromCenter >= 200) {
    shouldHavePokemart = rand() % 20 == 0;
  }

  int NSpivot, EWpivot;

  NSpivot = rand() % 8 + 6;
  EWpivot = rand() % 30 + 20;

  int y, x, dir;

  // Draw N to S path

  for (y = (pathAllowed[N] ? room->entrances[N]->y : room->entrances[N]->y + 1);
       y < NSpivot; y++) {
    room->tiles[y][room->entrances[N]->x].type = PATH;

    if (y == NSpivot - 3 && shouldHavePokemonCenter) {
      room->tiles[y][room->entrances[N]->x + 1].type = POKEMON_CENTER;
    }
  }

  dir = room->entrances[N]->x < room->entrances[S]->x ? 1 : -1;

  for (x = room->entrances[N]->x; x != room->entrances[S]->x; x += dir) {
    room->tiles[NSpivot][x].type = PATH;
  }

  for (y = NSpivot; y <= (pathAllowed[S] ? room->entrances[S]->y
                                         : room->entrances[S]->y - 1);
       y++) {
    room->tiles[y][room->entrances[S]->x].type = PATH;
  }

  // Draw W to E path

  for (x = (pathAllowed[W] ? room->entrances[W]->x : room->entrances[W]->x + 1);
       x < EWpivot; x++) {
    room->tiles[room->entrances[W]->y][x].type = PATH;

    if (x == EWpivot - 5 && shouldHavePokemart) {
      room->tiles[room->entrances[W]->y + 1][x].type = POKEMART;
    }
  }

  dir = room->entrances[W]->y < room->entrances[E]->y ? 1 : -1;

  for (y = room->entrances[W]->y; y != room->entrances[E]->y; y += dir) {
    room->tiles[y][EWpivot].type = PATH;
  }

  for (x = EWpivot; x <= (pathAllowed[E] ? room->entrances[E]->x
                                         : room->entrances[E]->x - 1);
       x++) {
    room->tiles[room->entrances[E]->y][x].type = PATH;
  }

  room->entrances[N]->type = ENTRANCE;
  room->entrances[E]->type = ENTRANCE;
  room->entrances[S]->type = ENTRANCE;
  room->entrances[W]->type = ENTRANCE;

  return 0;
}