#include "player.h"

#include <ncurses.h>

#include <cstdlib>

#include "../../util/debug/debug.h"
#include "../cohesivepokemon/cohesivepokemon.h"
#include "../game/game.h"
#include "../gamedata/gamedata.h"
#include "../menu/menu.h"
#include "../room/room.h"
#include "../trainer/trainer.h"

int canPlayerTravelTo(Player *player, Room *currentRoom, int x, int y) {
  if (currentRoom == NULL) {
    return 0;
  }

  int tileType = currentRoom->tiles[y][x].type;

  if (tileType == BOULDER || tileType == FOREST || tileType == WATER) {
    return 0;
  }

  if (currentRoom->trainers[y][x] != NULL) {
    if (currentRoom->trainers[y][x]->defeated == 0) {
      return 2;
    }

    return 0;
  }

  return 1;
}

int fly(int move, Game *game) {
  if (move == '\n') {
    int x, y;

    if (sscanf(game->player->input.c_str(), "%d %d", &x, &y) != 2) {
      setStatus(game,
                "Invalid fly parameters! Please enter two numbers: `<x> <y>`");
      game->player->input.clear();
      return 1;
    }

    // 200, 200 = 0, 0

    if (x < -200 || x > 200 || y < -200 || y > 200) {
      setStatus(game,
                "Invalid fly parameters! Please enter two numbers in bounds: "
                "`<x> <y>`");
      game->player->input.clear();
      return 1;
    }

    game->player->input.clear();

    x += 200;
    y += 200;

    Room *room = game->map.rooms[y][x];

    if (room == NULL) {
      room = (Room *)malloc(sizeof(Room));
      createRoom(&game->map, room, x, y);

      setStatus(game, "Your flight has landed!");
      game->player->flying = 0;

      game->map.rooms[y][x] = room;
      game->map.x = x;
      game->map.y = y;

      Tile *entrance = room->entrances[N];
      game->player->x = entrance->x;
      game->player->y = entrance->y + 1;

      return 0;
    }
  }

  game->player->input.push_back(move);

  return 1;
}

int Player::movePlayer(int move, Game *game) {
  if (move == 'q' || move == 'Q') {
    game->state = GAME_STATE_QUIT;
    return 0;
  }

  if (game->state == GAME_STATE_IN_MENU) {
    return game->menu.move(move, game);
  }

  Room *currentRoom = game->map.rooms[game->map.y][game->map.x];
  Player *player = game->player;

  if (player->flying) {
    return fly(move, game);
  }

  if (game->state == GAME_STATE_PLAYING) {
    int dx = 0, dy = 0;
    switch (move) {
      case 'B':
        setStatus(game, "Viewing Bag");
        prepareMenu(MENU_TYPE_BAG, &game->menu);
        game->state = GAME_STATE_IN_MENU;
        return 0;
      case 'f':
        setStatus(game, "Flying! Enter coordinates `<x> <y>` to land.");
        player->flying = 1;
        return 1;
      case 't':
        setStatus(game, "Viewing Menu");
        prepareMenu(MENU_TYPE_TRAINER_LIST, &game->menu);
        game->state = GAME_STATE_IN_MENU;
        return 0;
      case '5':
      case ' ':
      case '.':
        return 0;
      case '7':
      case 'y':
        dx = -1;
        dy = -1;
        break;
      case '8':
      case 'k':
        dy = -1;
        break;
      case '9':
      case 'u':
        dx = 1;
        dy = -1;
        break;
      case '6':
      case 'l':
        dx = 1;
        break;
      case '3':
      case 'n':
        dx = 1;
        dy = 1;
        break;
      case '2':
      case 'j':
        dy = 1;
        break;
      case '1':
      case 'b':
        dx = -1;
        dy = 1;
        break;
      case '4':
      case 'h':
        dx = -1;
        break;
      case '>': {
        int tileType = currentRoom->tiles[player->y][player->x].type;
        if (tileType == POKEMON_CENTER || tileType == POKEMART) {
          game->state = GAME_STATE_IN_MENU;
          prepareMenu(tileType == POKEMON_CENTER ? MENU_TYPE_POKEMON_CENTER
                                                 : MENU_TYPE_POKEMART,
                      &game->menu);
          setStatus(game, "Viewing Menu");
          return 0;
        }
        break;
      }
    }

    if (dx != 0 || dy != 0) {
      int travelStatus = canPlayerTravelTo(player, currentRoom, player->x + dx,
                                           player->y + dy);

      if (travelStatus == 2) {
        game->battle->startTrainerBattle(
            currentRoom->trainers[player->y + dy][player->x + dx]);
        return 0;
      }

      if (travelStatus) {
        player->x += dx;
        player->y += dy;

        if (game->map.rooms[game->map.y][game->map.x]
                ->tiles[player->y][player->x]
                .type == ENTRANCE) {
          int toDirection;

          int x = game->map.x;
          int y = game->map.y;

          if (player->x == 0) {
            toDirection = E;
            x--;
          }

          if (player->x == ROOM_WIDTH - 1) {
            toDirection = W;
            x++;
          }

          if (player->y == 0) {
            toDirection = S;
            y--;
          }

          if (player->y == ROOM_HEIGHT - 1) {
            toDirection = N;
            y++;
          }

          Room *room = game->map.rooms[y][x];

          if (room == NULL) {
            room = (Room *)malloc(sizeof(Room));
            createRoom(&game->map, room, x, y);
          }

          game->map.rooms[y][x] = room;
          game->map.y = y;
          game->map.x = x;

          Tile *entrance = room->entrances[toDirection];

          player->x = entrance->x;
          player->y = entrance->y;

          switch (toDirection) {
            case N:
              player->y++;
              break;
            case E:
              player->x--;
              break;
            case S:
              player->y--;
              break;
            case W:
              player->x++;
              break;
          }
        }

        Tile *tile = &game->map.rooms[game->map.y][game->map.x]
                          ->tiles[player->y][player->x];

        if (tile->type == TALL_GRASS || tile->type == TALL_GRASS_2) {
          game->player->chanceEncounter();
        }

        return 0;
      }

      setStatus(game, "Can't travel! Something is in the way!");
    }
  }

  return 1;
}

void Player::generateStarters() {
  int i;

  for (i = 0; i < 3; i++) {
    int pokemonId = rand() % POSSIBLE_POKEMON + 1;
    this->possibleStarters[i] = pokemonId;
  }
}

void Player::chanceEncounter() {
  if (rand() % 10 != 0) {
    return;
  }

  int pokemonId = rand() % POSSIBLE_POKEMON + 1;

  CohesivePokemon *pokemon = new CohesivePokemon(gameData->pokemon[pokemonId]);

  game->battle->startPokemonBattle(pokemon);
}

bool Player::allPokemonFainted() {
  for (size_t i = 0; i < this->pokemon.size(); i++) {
    if (this->pokemon[i]->HP > 0) {
      return false;
    }
  }

  return true;
}
