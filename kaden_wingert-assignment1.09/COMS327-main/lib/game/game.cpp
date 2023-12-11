#include "game.h"

#include <ncurses.h>

#include <cstdlib>
#include <cstring>

#include "../render/render.h"
#include "../room/room.h"
#include "../trainer/trainer.h"

void placePlayerRandomly(Map *map, Player *player) {
  Room *currentRoom = map->rooms[map->y][map->x];

  int y = rand() % (ROOM_HEIGHT - 2) + 1;
  int x = rand() % (ROOM_WIDTH - 2) + 1;

  while (currentRoom->tiles[y][x].type != PATH ||
         currentRoom->trainers[y][x] != NULL) {
    y = rand() % (ROOM_HEIGHT - 2) + 1;
    x = rand() % (ROOM_WIDTH - 2) + 1;
  }

  player->x = x;
  player->y = y;
}

void setStatus(Game *game, const char *status) {
  memset(game->status, 0, sizeof(game->status));
  strcpy(game->status, status);
}

int createGame(Game *game, int trainersPerRoom) {
  createMap(&game->map, trainersPerRoom);

  game->player = new Player();

  setStatus(game, "");

  game->state = GAME_STATE_PLAYING;

  game->battle = new Battle();

  return 0;
}

int destroyGame(Game *game) {
  destroyMap(&game->map);

  return 0;
}

void startLoop(Game *game) {
  placePlayerRandomly(&game->map, game->player);

  renderGame(game);

  setStatus(game, "It's your move!");

  while (game->state != GAME_STATE_QUIT) {
    renderGame(game);

    int move = getch();

    while (game->player->movePlayer(move, game)) {
      renderGame(game);
      move = getch();
    }

    if (game->player->immune) {
      game->player->immune = false;
      continue;
    }

    if (game->state == GAME_STATE_PLAYING) {
      Trainer *trainerToBattle = NULL;

      moveTrainers(game->map.rooms[game->map.y][game->map.x], game->player,
                   &trainerToBattle);

      if (trainerToBattle != NULL) {
        game->battle->startTrainerBattle(trainerToBattle);

        continue;
      }

      setStatus(game, "It's your move!");
    }
  }
}