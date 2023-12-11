#include "render.h"

#include <ncurses.h>

#include <cstdlib>

#include "../game/game.h"
#include "../room/room.h"
#include "../tile/tile.h"
#include "../trainer/trainer.h"

void initializeColors() {
  start_color();

  init_pair(COLOR_PLAYER, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_TRAINER, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GRASS, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_TALL_GRASS, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_FOREST, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_WATER, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_BOULDER, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_PATH, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_ENTRANCE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_POKEMON_CENTER, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_POKEMART, COLOR_YELLOW, COLOR_BLACK);
}

void startUsingColor(int color) { attron(COLOR_PAIR(color)); }
void stopUsingColor(int color) { attroff(COLOR_PAIR(color)); }

void useDefaultColor() { attroff(COLOR_PAIR(0)); }

int getTileColor(Tile tile) {
  switch (tile.type) {
    case EMPTY:
      break;
    case GRASS:
    case GRASS_2:
      return COLOR_GRASS;
    case TALL_GRASS:
    case TALL_GRASS_2:
      return COLOR_TALL_GRASS;
    case FOREST:
      return COLOR_FOREST;
    case WATER:
      return COLOR_WATER;
    case BOULDER:
      return COLOR_BOULDER;
    case PATH:
      return COLOR_PATH;
    case ENTRANCE:
      return COLOR_ENTRANCE;
    case POKEMON_CENTER:
      return COLOR_POKEMON_CENTER;
    case POKEMART:
      return COLOR_POKEMART;
  }

  return 0;
}

void renderGame(Game *game) {
  clear();
  useDefaultColor();

  mvprintw(0, 0, game->status);

  int y, x;

  for (y = 0; y < ROOM_HEIGHT; y++) {
    for (x = 0; x < ROOM_WIDTH; x++) {
      if (x == game->player->x && y == game->player->y) {
        startUsingColor(COLOR_PLAYER);
        mvaddch(y + 1, x, '@');
        stopUsingColor(COLOR_PLAYER);
        continue;
      }

      Room room = *game->map.rooms[game->map.y][game->map.x];

      int tileColor = getTileColor(room.tiles[y][x]);

      startUsingColor(tileColor);
      mvaddch(y + 1, x, getTileCharacter(room.tiles[y][x]));
      stopUsingColor(tileColor);

      if (room.trainers[y][x] != NULL) {
        startUsingColor(COLOR_TRAINER);
        mvaddch(y + 1, x, getTrainerCharacter(room.trainers[y][x]));
        stopUsingColor(COLOR_TRAINER);
        continue;
      }
    }
  }

  if (game->state == GAME_STATE_IN_MENU) {
    game->menu.render(game);
  }

  if (game->player->input != "") {
    mvprintw(ROOM_HEIGHT + 1, 0, "Input: %s", game->player->input.c_str());
  }

  refresh();
}