#include "../cohesivepokemon/cohesivepokemon.h"
#include "../game/game.h"
#include "../menu/menu.h"
#include "../trainer/trainer.h"

void Battle::startTrainerBattle(Trainer* against) {
  this->againstTrainer = against;
  this->type = BATTLE_TYPE_TRAINER;
  this->subMenu = BATTLE_MENU_CHOOSE;

  for (size_t i = 0; i < against->pokemon.size(); i++) {
    CohesivePokemon* pokemon = against->pokemon[i];

    if (pokemon->HP <= 0) {
      continue;
    }

    this->againstPokemon = pokemon;
    break;
  }

  for (size_t i = 0; i < game->player->pokemon.size(); i++) {
    CohesivePokemon* pokemon = game->player->pokemon[i];

    if (pokemon->HP <= 0) {
      continue;
    }

    this->playerPokemon = pokemon;
    break;
  }

  setStatus(game, "Battling a trainer");

  prepareMenu(MENU_TYPE_BATTLE, &game->menu);

  game->state = GAME_STATE_IN_MENU;
}

void Battle::startPokemonBattle(CohesivePokemon* against) {
  this->againstPokemon = against;
  this->type = BATTLE_TYPE_POKEMON;
  this->subMenu = BATTLE_MENU_CHOOSE;

  for (size_t i = 0; i < game->player->pokemon.size(); i++) {
    CohesivePokemon* pokemon = game->player->pokemon[i];

    if (pokemon->HP <= 0) {
      continue;
    }

    this->playerPokemon = pokemon;
    break;
  }

  setStatus(game, "Battling a wild pokemon");

  prepareMenu(MENU_TYPE_BATTLE, &game->menu);

  game->state = GAME_STATE_IN_MENU;
}
