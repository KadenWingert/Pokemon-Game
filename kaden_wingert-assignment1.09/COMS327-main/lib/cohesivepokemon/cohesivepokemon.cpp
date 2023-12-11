#include "cohesivepokemon.h"

#include <cmath>

int CohesivePokemon::generateIV(Statum statum) {
  PokemonStat baseStat;

  for (size_t i = 0; i < gameData->pokemonStats.size(); i++) {
    baseStat = gameData->pokemonStats[i];
    if (baseStat.pokemon_id == this->id && baseStat.stat_id == statum) {
      break;
    }
  }

  int iv = rand() % 16;

  int constant = statum == STATUM_HP ? this->level + 10 : 5;

  return floor(((((baseStat.base_stat) + iv) * 2) * this->level) / 100) +
         constant;
}

void CohesivePokemon::setLevel(int level) {
  this->level = level;

  this->maxHP = this->generateIV(STATUM_HP);
  this->HP = this->maxHP;
  this->attack = this->generateIV(STATUM_ATTACK);
  this->defense = this->generateIV(STATUM_DEFENSE);
  this->speed = this->generateIV(STATUM_SPEED);
  this->specialAttack = this->generateIV(STATUM_SPECIAL_ATTACK);
  this->specialDefense = this->generateIV(STATUM_SPECIAL_DEFENSE);
}

void CohesivePokemon::decideMoves() {
  std::vector<PokemonMove> possibleMoves;

  while (possibleMoves.size() == 0) {
    for (size_t i = 0; i < gameData->pokemonMoves.size(); i++) {
      PokemonMove possibleMove = gameData->pokemonMoves[i];

      if (this->level >= possibleMove.level &&
          possibleMove.pokemon_id == this->id) {
        possibleMoves.push_back(gameData->pokemonMoves[i]);
      }
    }

    if (possibleMoves.size() == 0) {
      this->setLevel(this->level + 1);
    }
  }

  int numMoves = possibleMoves.size() < 2 ? possibleMoves.size() : 2;

  for (int i = 0; i < numMoves; i++) {
    int randomIndex = rand() % possibleMoves.size();
    this->moves.push_back(gameData->moves[possibleMoves[randomIndex].move_id]);
    possibleMoves.erase(possibleMoves.begin() + randomIndex);
  }
}

int CohesivePokemon::calculateDamage(CohesivePokemon* against, Move move) {
  int left = ((((((2 * this->level) / 5) + 2) * move.power *
                (this->attack / against->defense)) /
               50) +
              2);

  double critical = rand() % 256 < (this->speed * 2 / 5) ? 1.5 : 1;
  int random = rand() % 16 + 85;
  double stab = move.type_id == this->typeId ? 1.5 : 1;

  return left * critical * random * stab;
}

void CohesivePokemon::decreaseHP(int amount) {
  this->HP -= amount;

  if (this->HP < 0) {
    this->HP = 0;
  }
}
