#ifndef POKEMON_MOVE_H
#define POKEMON_MOVE_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class PokemonMove {
 public:
  int pokemon_id;
  int version_group_id;
  int move_id;
  int pokemon_move_method_id;
  int level;
  int order;
  PokemonMove& fromCSV(std::vector<std::string> data) {
    this->pokemon_id = safeStoi(data[0]);
    this->version_group_id = safeStoi(data[1]);
    this->move_id = safeStoi(data[2]);
    this->pokemon_move_method_id = safeStoi(data[3]);
    this->level = safeStoi(data[4]);
    this->order = safeStoi(data[5]);

    return *this;
  };
};

#endif