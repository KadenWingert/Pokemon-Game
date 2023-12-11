#include "menu.h"

const char *menuHeaders[] = {"Trainer List", "Pokemon Center", "Pokemart",
                             "Battle", "Choose Starter Pokemon"};

#include <ncurses.h>

#include <cstdlib>
#include <cstring>

#include "../battle/battle.h"
#include "../cohesivepokemon/cohesivepokemon.h"
#include "../game/game.h"
#include "../gamedata/gamedata.h"
#include "../room/room.h"
#include "../trainer/trainer.h"

void renderBaseMenu(const char *title) {
  int boxX = (WINDOW_WIDTH - MENU_WIDTH) / 2;
  int boxY = (WINDOW_HEIGHT - MENU_HEIGHT) / 2;

  int y, x;

  for (y = 0; y < MENU_HEIGHT; y++) {
    for (x = 0; x < MENU_WIDTH; x++) {
      if ((y == 0 || y == MENU_HEIGHT - 1) && (x == 0 || x == MENU_WIDTH - 1)) {
        mvaddch(boxY + y, boxX + x, '+');
        continue;
      }

      if (y == 0 || y == MENU_HEIGHT - 1) {
        mvaddch(boxY + y, boxX + x, '-');
        continue;
      }

      if (x == 0 || x == MENU_WIDTH - 1) {
        mvaddch(boxY + y, boxX + x, '|');
        continue;
      }

      mvaddch(boxY + y, boxX + x, ' ');
    }
  }

  mvprintw(boxY + 1, (WINDOW_WIDTH - strlen(title)) / 2, title);
}

void calculateStartPosition(int *x, int *y) {
  *x = (WINDOW_WIDTH - MENU_WIDTH) / 2 + 2;
  *y = (WINDOW_HEIGHT - MENU_HEIGHT) / 2 + 2;
}

void renderTrainerList(Game *game) {
  renderBaseMenu(menuHeaders[0]);

  int x, y;
  calculateStartPosition(&x, &y);

  Player *player = game->player;

  int j, i;
  int seen = 0;

  for (j = 0; j < ROOM_HEIGHT; j++) {
    for (i = 0; i < ROOM_WIDTH; i++) {
      if (y > MENU_HEIGHT) {
        return;
      }

      Trainer *trainer =
          game->map.rooms[game->map.y][game->map.x]->trainers[j][i];

      if (trainer == NULL) {
        continue;
      }

      if (seen < game->menu.position) {
        seen++;
        continue;
      }

      char name = getTrainerCharacter(trainer);

      int horizDist = player->x - i;
      int vertDist = player->y - j;

      mvprintw(y, x, "%c, %d %s and %d %s", name, abs(horizDist),
               horizDist > 0 ? "west" : "east", abs(vertDist),
               vertDist > 0 ? "north" : "south");
      y++;
    }
  }
}

int moveTrainerList(int move, Game *game) {
  switch (move) {
    case 27:  // Escape
      game->state = GAME_STATE_PLAYING;
      return 0;
    case KEY_UP:
      if (game->menu.position > 0) {
        game->menu.position--;
      }
      return 0;
    case KEY_DOWN:
      if (game->menu.position < game->map.trainersPerRoom - MENU_HEIGHT + 3) {
        game->menu.position++;
      }
      return 0;
  }

  return 1;
}

void renderPokemonCenter(Game *game) {
  renderBaseMenu(menuHeaders[1]);

  int x, y;
  calculateStartPosition(&x, &y);

  mvprintw(y, x, "Your Pokemon were restored to full health!");
  mvprintw(y + 1, x, "Press < to exit.");

  for (size_t i = 0; i < game->player->pokemon.size(); i++) {
    game->player->pokemon[i]->HP = game->player->pokemon[i]->maxHP;
  }
}

int movePokemonCenter(int move, Game *game) {
  switch (move) {
    case '<':
      game->state = GAME_STATE_PLAYING;
      return 0;
  }

  return 1;
}

void renderPokemart(Game *game) {
  renderBaseMenu(menuHeaders[2]);

  int x, y;
  calculateStartPosition(&x, &y);

  mvprintw(y, x, "Your supplies were replenished!");
  mvprintw(y + 1, x, "Press < to exit.");

  game->player->pokeballs = 10;
  game->player->potions = 3;
  game->player->revives = 3;
}

int movePokemart(int move, Game *game) {
  switch (move) {
    case '<':
      game->state = GAME_STATE_PLAYING;
      return 0;
  }

  return 1;
}

void renderBattle(Game *game) {
  renderBaseMenu(menuHeaders[3]);

  int x, y;
  calculateStartPosition(&x, &y);

  if (game->player->allPokemonFainted()) {
    mvprintw(y, x, "All of your Pokemon have fainted!");
    mvprintw(y + 1, x, "Please visit a Pokemon Center to restore them.");
    mvprintw(y + 2, x, "Press < to exit.");

    return;
  }

  switch (game->battle->type) {
    case BATTLE_TYPE_TRAINER: {
      Trainer *against = game->battle->againstTrainer;

      mvprintw(y, x, "You are battling trainer %c!",
               getTrainerCharacter(against));

      break;
    }
    case BATTLE_TYPE_POKEMON: {
      CohesivePokemon *against = game->battle->againstPokemon;

      mvprintw(y, x, "You are battling a wild %s!",
               against->identifier.c_str());

      break;
    }
  }

  mvprintw(y + 1, x, "----------------------------------------");

  mvprintw(y + 2, x, "Enemy Pokemon: %s",
           game->battle->againstPokemon->identifier.c_str());
  mvprintw(y + 3, x, "Enemy HP: %d/%d", game->battle->againstPokemon->HP,
           game->battle->againstPokemon->maxHP);

  mvprintw(y + 4, x, "----------------------------------------");

  mvprintw(y + 5, x, "Your Pokemon: %s",
           game->battle->playerPokemon->identifier.c_str());
  mvprintw(y + 6, x, "Your HP: %d/%d", game->battle->playerPokemon->HP,
           game->battle->playerPokemon->maxHP);

  mvprintw(y + 7, x, "----------------------------------------");

  if (game->battle->subMenu == BATTLE_MENU_CHOOSE) {
    mvprintw(y + 8, x, "1. Fight");
    mvprintw(y + 9, x, "2. Bag");
    bool canRun = game->battle->type == BATTLE_TYPE_POKEMON;
    if (canRun) {
      mvprintw(y + 10, x, "3. Run");
    }
    mvprintw(y + (canRun ? 11 : 10), x, "4. Pokemon");
    return;
  }

  if (game->battle->subMenu == BATTLE_MENU_FIGHT) {
    size_t i;

    for (i = 0; i < game->battle->playerPokemon->moves.size(); i++) {
      mvprintw(y + 8 + i, x, "%d. %s", i + 1,
               game->battle->playerPokemon->moves[i].identifier.c_str());
    }

    return;
  }

  if (game->battle->subMenu == BATTLE_MENU_POKEMON) {
    size_t i;

    for (i = 0; i < game->player->pokemon.size(); i++) {
      mvprintw(y + 8 + i, x, "%d. %s (%d/%d)", i + 1,
               game->player->pokemon[i]->identifier.c_str(),
               game->player->pokemon[i]->HP, game->player->pokemon[i]->maxHP);
    }

    return;
  }

  if (game->battle->subMenu == BATTLE_MENU_BAG) {
    bool pokeballsAvailable = game->battle->type == BATTLE_TYPE_POKEMON;
    if (pokeballsAvailable) {
      mvprintw(y + 8, x, "1. Pokeball (x%d)", game->player->pokeballs);
    }
    mvprintw(y + (pokeballsAvailable ? 9 : 8), x, "2. Potion (x%d)",
             game->player->potions);
    mvprintw(y + (pokeballsAvailable ? 10 : 9), x, "3. Revive (x%d)",
             game->player->revives);
    return;
  }
}

int moveBattle(int move, Game *game) {
  if (game->player->allPokemonFainted()) {
    switch (move) {
      case '<':
        game->state = GAME_STATE_PLAYING;
        game->player->immune = true;
        return 0;
    }

    return 1;
  }

  Move *myMove = NULL;

  if (game->battle->subMenu != BATTLE_MENU_CHOOSE && move == 27) {
    game->battle->subMenu = BATTLE_MENU_CHOOSE;
    return 0;
  }

  if (game->battle->subMenu == BATTLE_MENU_CHOOSE) {
    switch (move) {
      case '1':
        if (game->battle->subMenu == BATTLE_MENU_CHOOSE) {
          game->battle->subMenu = BATTLE_MENU_FIGHT;
          return 0;
        }
        break;
      case '2':
        if (game->battle->subMenu == BATTLE_MENU_CHOOSE) {
          game->battle->subMenu = BATTLE_MENU_BAG;
          return 0;
        }
        break;
      case '3':
        if (game->battle->type == BATTLE_TYPE_TRAINER) {
          return 1;
        }

        if (game->battle->subMenu == BATTLE_MENU_CHOOSE) {
          game->state = GAME_STATE_PLAYING;

          int chance = rand() % 100;
          if (chance > 50) {
            setStatus(game, "You ran away!");
            game->player->immune = true;
            return 0;
          } else {
            setStatus(game, "You couldn't run fast enough!");
            goto start_enemy_turn;
          }
        }

        break;
      case '4':
        if (game->battle->subMenu == BATTLE_MENU_CHOOSE) {
          game->battle->subMenu = BATTLE_MENU_POKEMON;
          return 0;
        }
        break;
    }
  }

  if (game->battle->subMenu == BATTLE_MENU_BAG) {
    switch (move) {
      case '1':
        if (game->battle->type == BATTLE_TYPE_TRAINER) {
          return 1;
        }

        if (game->player->pokeballs > 0) {
          game->player->pokeballs--;

          if (game->player->pokemon.size() < 6) {
            game->player->pokemon.push_back(
                new CohesivePokemon(*game->battle->againstPokemon));
            setStatus(game, "You caught the wild pokemon!");
          } else {
            setStatus(game, "The wild pokemon fled!");
          }

          game->battle->subMenu = BATTLE_MENU_CHOOSE;
          game->state = GAME_STATE_PLAYING;
        } else {
          setStatus(game, "You don't have any pokeballs!");
        }

        return 0;

      case '2':
        if (game->player->potions > 0) {
          game->player->potions--;
          game->battle->playerPokemon->HP += 20;
          if (game->battle->playerPokemon->HP >
              game->battle->playerPokemon->maxHP) {
            game->battle->playerPokemon->HP =
                game->battle->playerPokemon->maxHP;
          }
          setStatus(game, "You used a potion!");
          game->battle->subMenu = BATTLE_MENU_CHOOSE;
          goto start_enemy_turn;
        } else {
          setStatus(game, "You don't have any potions!");
        }
        return 0;

      case '3':
        if (game->player->revives > 0) {
          game->player->revives--;
          game->battle->playerPokemon->HP = game->battle->playerPokemon->maxHP;
          setStatus(game, "You used a revive!");
          game->battle->subMenu = BATTLE_MENU_CHOOSE;
          goto start_enemy_turn;
        } else {
          setStatus(game, "You don't have any revives!");
        }
        return 0;
    }
  }

  if (game->battle->subMenu == BATTLE_MENU_POKEMON) {
    size_t pokemonIndex = move - '1';

    if (pokemonIndex < 0 || pokemonIndex >= game->player->pokemon.size()) {
      setStatus(game, "Invalid pokemon! Please choose another.");
      return 1;
    }

    CohesivePokemon *pokemon = game->player->pokemon[pokemonIndex];

    if (pokemon->HP <= 0) {
      setStatus(game, "That pokemon has fainted! Please choose another.");
      return 1;
    }

    game->battle->playerPokemon = pokemon;
    game->battle->subMenu = BATTLE_MENU_CHOOSE;

    char status[60];
    snprintf(status, 17 + pokemon->identifier.size(), "You sent out %s!",
             pokemon->identifier.c_str());
    setStatus(game, status);

    goto start_enemy_turn;
  }

  if (game->battle->subMenu == BATTLE_MENU_FIGHT) {
    size_t moveIndex = move - '1';

    if (moveIndex < 0 ||
        moveIndex >= game->battle->playerPokemon->moves.size()) {
      setStatus(game, "Invalid move! Please choose another.");
      return 1;
    }

    myMove = &game->battle->playerPokemon->moves[moveIndex];
    game->battle->subMenu = BATTLE_MENU_CHOOSE;
  }

start_enemy_turn:
  int ran = rand() % 100;
  int enemyChoice = 2;

  if (ran > 90) {
    enemyChoice = 1;
  } else if (ran > 80) {
    enemyChoice = 0;
  }

  if (enemyChoice == 0) {
    setStatus(game, "The enemy used a potion!");
    game->battle->againstPokemon->HP += 20;
    if (game->battle->againstPokemon->HP >
        game->battle->againstPokemon->maxHP) {
      game->battle->againstPokemon->HP = game->battle->againstPokemon->maxHP;
    }

    return 0;
  }

  if (enemyChoice == 1) {
    setStatus(game, "The enemy used a revive!");
    game->battle->againstPokemon->HP = game->battle->againstPokemon->maxHP;

    return 0;
  }

  int enemyMoveIndex = rand() % game->battle->againstPokemon->moves.size();
  Move enemyMove = game->battle->againstPokemon->moves[enemyMoveIndex];

  if (myMove != NULL && myMove->priority >= enemyMove.priority) {
    int damage = game->battle->playerPokemon->calculateDamage(
        game->battle->againstPokemon, *myMove);

    char status[60];
    snprintf(status, 17 + myMove->identifier.size(), "You used %s!",
             myMove->identifier.c_str());
    setStatus(game, status);

    game->battle->againstPokemon->decreaseHP(damage);
  }

  if (game->battle->againstPokemon->HP > 0) {
    int damage = game->battle->againstPokemon->calculateDamage(
        game->battle->playerPokemon, enemyMove);

    char status[60];
    snprintf(status, 17 + enemyMove.identifier.size(), "The enemy used %s!",
             enemyMove.identifier.c_str());
    setStatus(game, status);

    game->battle->playerPokemon->decreaseHP(damage);
  }

  if (myMove != NULL && myMove->priority < enemyMove.priority) {
    int damage = game->battle->playerPokemon->calculateDamage(
        game->battle->againstPokemon, *myMove);

    char status[60];
    snprintf(status, 17 + myMove->identifier.size(), "You used %s!",
             myMove->identifier.c_str());
    setStatus(game, status);

    game->battle->againstPokemon->decreaseHP(damage);
  }

  // check if the enemy pokemon is dead
  // if they are, check if all the pokemon are dead (only for trainer battles)
  // if all are dead, we won
  // if there is one alive, switch to that one (only for trainer battles)

  // if my pokemon is dead, check if all the pokemon are dead
  // if all are dead, we lost
  // if there is one alive, open the pokemon menu

  if (game->battle->againstPokemon->HP <= 0) {
    game->battle->againstPokemon->HP = 0;

    if (game->battle->type == BATTLE_TYPE_TRAINER) {
      bool allDead = true;

      for (size_t i = 0; i < game->battle->againstTrainer->pokemon.size(); i++) {
        CohesivePokemon *pokemon = game->battle->againstTrainer->pokemon[i];

        if (pokemon->HP > 0) {
          allDead = false;
          break;
        }
      }

      if (allDead) {
        setStatus(game, "You won the battle!");
        game->state = GAME_STATE_PLAYING;
        game->player->immune = true;
        game->battle->againstTrainer->defeated = true;
        return 0;
      }

      for (size_t i = 0; i < game->battle->againstTrainer->pokemon.size(); i++) {
        CohesivePokemon *pokemon = game->battle->againstTrainer->pokemon[i];

        if (pokemon->HP <= 0) {
          continue;
        }

        game->battle->againstPokemon = pokemon;
        break;
      }

      setStatus(game, "The enemy sent out another pokemon!");
      return 0;
    }

    setStatus(game, "You won the battle!");
    game->state = GAME_STATE_PLAYING;
    game->player->immune = true;
    return 0;
  }

  if (game->battle->playerPokemon->HP <= 0) {
    game->battle->playerPokemon->HP = 0;

    if (game->player->allPokemonFainted()) {
      setStatus(game, "You lost the battle!");
      game->state = GAME_STATE_PLAYING;
      game->player->immune = true;
      return 0;
    }

    game->battle->subMenu = BATTLE_MENU_POKEMON;
    setStatus(game, "Your pokemon fainted! Please choose another.");
    return 0;
  }

  return 0;
}

void renderChooseStarter(Game *game) {
  renderBaseMenu(menuHeaders[4]);

  int x, y;
  calculateStartPosition(&x, &y);

  for (int i = 0; i < 3; i++) {
    mvprintw(y + i, x, "%d. %s", (i + 1),
             gameData->pokemon[game->player->possibleStarters[i]]
                 .identifier.c_str());
  }

  mvprintw(y + 5, x, "Choose your starter by hitting 1, 2, or 3!");
}

int moveChooseStarter(int move, Game *game) {
  switch (move) {
    case '1':
      game->player->pokemon.push_back(new CohesivePokemon(
          gameData->pokemon[game->player->possibleStarters[0]]));
      game->state = GAME_STATE_PLAYING;
      return 0;
    case '2':
      game->player->pokemon.push_back(new CohesivePokemon(
          gameData->pokemon[game->player->possibleStarters[1]]));
      game->state = GAME_STATE_PLAYING;
      return 0;
    case '3':
      game->player->pokemon.push_back(new CohesivePokemon(
          gameData->pokemon[game->player->possibleStarters[2]]));
      game->state = GAME_STATE_PLAYING;
      return 0;
  }

  return 1;
}

static int bagMenu = 0;

int moveBag(int move, Game *game) {
  if (bagMenu == 0) {
    switch (move) {
      case 27:
        game->state = GAME_STATE_PLAYING;
        return 0;
      case '1':
        bagMenu = 1;
        return 0;
      case '2':
        bagMenu = 2;
        return 0;
    }
    return 1;
  }

  if (bagMenu == 1) {
    if (move == 27) {
      bagMenu = 0;
      return 0;
    }

    if (game->player->potions == 0) {
      setStatus(game, "You don't have any potions!");
      return 1;
    }

    size_t pokemonIndex = move - '1';

    if (pokemonIndex < 0 || pokemonIndex >= game->player->pokemon.size()) {
      setStatus(game, "Invalid pokemon! Please choose another.");
      return 1;
    }

    CohesivePokemon *pokemon = game->player->pokemon[pokemonIndex];

    pokemon->HP += 20;

    if (pokemon->HP > pokemon->maxHP) {
      pokemon->HP = pokemon->maxHP;
    }

    setStatus(game, "You used a potion!");
    game->player->potions--;

    return 0;
  }

  if (bagMenu == 2) {
    if (move == 27) {
      bagMenu = 0;
      return 0;
    }

    if (game->player->revives == 0) {
      setStatus(game, "You don't have any revives!");
      return 1;
    }

    size_t pokemonIndex = move - '1';

    if (pokemonIndex < 0 || pokemonIndex >= game->player->pokemon.size()) {
      setStatus(game, "Invalid pokemon! Please choose another.");
      return 1;
    }

    CohesivePokemon *pokemon = game->player->pokemon[pokemonIndex];

    pokemon->HP = pokemon->maxHP;

    setStatus(game, "You used a revive!");
    game->player->revives--;

    return 0;
  }

  return 1;
}

void renderBag(Game *game) {
  renderBaseMenu("Bag");

  int x, y;
  calculateStartPosition(&x, &y);

  if (bagMenu == 0) {
    mvprintw(y, x, "1. Use potions (x%d)", game->player->potions);
    mvprintw(y + 1, x, "2. Use revives (x%d)", game->player->revives);
    mvprintw(y + 2, x, "Pokeballs (x%d)", game->player->pokeballs);
  }

  if (bagMenu == 1) {
    mvprintw(y, x, "Choose a pokemon to use a potion on (%d left):",
             game->player->potions);

    size_t i;

    for (i = 0; i < game->player->pokemon.size(); i++) {
      mvprintw(y + i + 1, x, "%d. %s (%d/%d)", i + 1,
               game->player->pokemon[i]->identifier.c_str(),
               game->player->pokemon[i]->HP, game->player->pokemon[i]->maxHP);
    }
  }

  if (bagMenu == 2) {
    mvprintw(y, x, "Choose a pokemon to use a revive on (%d left):",
             game->player->revives);

    size_t i;

    for (i = 0; i < game->player->pokemon.size(); i++) {
      mvprintw(y + i + 1, x, "%d. %s (%d/%d)", i + 1,
               game->player->pokemon[i]->identifier.c_str(),
               game->player->pokemon[i]->HP, game->player->pokemon[i]->maxHP);
    }
  }
}

int prepareMenu(MenuType type, Menu *menu) {
  menu->type = type;
  menu->position = 0;

  switch (type) {
    case MENU_TYPE_TRAINER_LIST:
      menu->move = moveTrainerList;
      menu->render = renderTrainerList;
      break;
    case MENU_TYPE_POKEMON_CENTER:
      menu->move = movePokemonCenter;
      menu->render = renderPokemonCenter;
      break;
    case MENU_TYPE_POKEMART:
      menu->move = movePokemart;
      menu->render = renderPokemart;
      break;
    case MENU_TYPE_BATTLE:
      menu->move = moveBattle;
      menu->render = renderBattle;
      break;
    case MENU_TYPE_CHOOSE_STARTER:
      menu->move = moveChooseStarter;
      menu->render = renderChooseStarter;
      break;
    case MENU_TYPE_BAG:
      menu->move = moveBag;
      menu->render = renderBag;
      break;
  }

  return 0;
}
