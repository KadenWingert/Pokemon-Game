#ifndef POKEMON_TYPE_H
#define POKEMON_TYPE_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class PokemonType {
 public:
  int pokemon_id;
  int type_id;
  int slot;

  PokemonType& fromCSV(std::vector<std::string> data) {
    this->pokemon_id = safeStoi(data[0]);
    this->type_id = safeStoi(data[1]);
    this->slot = safeStoi(data[2]);

    return *this;
  }
};

#endif