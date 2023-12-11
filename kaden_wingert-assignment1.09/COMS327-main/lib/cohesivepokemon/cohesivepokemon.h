#ifndef CAPTURED_POKEMON_H
#define CAPTURED_POKEMON_H

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "../game/game.h"
#include "../gamedata/gamedata.h"

typedef enum { GENDER_MALE, GENDER_FEMALE } Gender;

typedef enum {
  STATUM_HP,
  STATUM_ATTACK,
  STATUM_DEFENSE,
  STATUM_SPEED,
  STATUM_SPECIAL_ATTACK,
  STATUM_SPECIAL_DEFENSE
} Statum;

class CohesivePokemon : public Pokemon {
 private:
  int level;
  int generateIV(Statum statum);
  void setLevel(int level);
  void decideMoves();

 public:
  int HP;
  int maxHP;
  int attack;
  int defense;
  int speed;
  int specialAttack;
  int specialDefense;
  Gender gender;
  bool shiny;
  int typeId;
  std::string type;
  std::vector<Move> moves;
  CohesivePokemon(Pokemon pokemon) : Pokemon(pokemon) {
    this->shiny = rand() % 8912 == 0;
    this->gender = rand() % 2 ? GENDER_MALE : GENDER_FEMALE;

    for (size_t i = 0; i < gameData->pokemonTypes.size(); i++) {
      if (gameData->pokemonTypes[i].pokemon_id == this->id) {
        this->typeId = gameData->pokemonTypes[i].type_id;
        this->type = gameData->typeNames[this->typeId - 1].name;
        break;
      }
    }

    int manhattanDistance = abs((game->map.x - MAP_WIDTH) / 2) +
                            abs((game->map.y - MAP_HEIGHT) / 2);

    if (manhattanDistance < 2) {
      manhattanDistance = 2;
    }

    if (manhattanDistance <= 200) {
      this->setLevel(rand() % (manhattanDistance / 2) + 1);
    } else {
      this->setLevel(rand() % (100 - (manhattanDistance - 200) / 2) +
                     (manhattanDistance - 200) / 2);
    }

    this->decideMoves();
  }

  int getLevel() { return this->level; }

  int calculateDamage(CohesivePokemon* against, Move move);

  void decreaseHP(int amount);
};

#endif
