#ifndef POKEMON_STAT_H
#define POKEMON_STAT_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class PokemonStat {
 public:
  int pokemon_id;
  int stat_id;
  int base_stat;
  int effort;

  PokemonStat& fromCSV(std::vector<std::string> data) {
    this->pokemon_id = safeStoi(data[0]);
    this->stat_id = safeStoi(data[1]);
    this->base_stat = safeStoi(data[2]);
    this->effort = safeStoi(data[3]);

    return *this;
  }
};

#endif