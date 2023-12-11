#ifndef GAME_H
#define GAME_H

#include "../battle/battle.h"
#include "../map/map.h"
#include "../menu/menu.h"
#include "../player/player.h"

typedef enum GameState {
  GAME_STATE_PLAYING,
  GAME_STATE_IN_MENU,
  GAME_STATE_QUIT
} GameState;

class Game {
 public:
  Map map;
  Player *player;
  int seed;
  char status[128];
  GameState state;
  Menu menu;
  Battle *battle;
};

extern Game *game;

int createGame(Game *game, int trainersPerRoom);
int destroyGame(Game *game);
void startLoop(Game *game);
void setStatus(Game *game, const char *status);

#endif