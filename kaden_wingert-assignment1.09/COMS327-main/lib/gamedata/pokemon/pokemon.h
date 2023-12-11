#ifndef POKEMON_H
#define POKEMON_H

#define POSSIBLE_POKEMON 898

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class Pokemon {
 public:
  int id;
  std::string identifier;
  int species_id;
  int height;
  int weight;
  int base_experience;
  int order;
  bool is_default;

  Pokemon& fromCSV(std::vector<std::string> data) {
    this->id = safeStoi(data[0]);
    this->identifier = data[1];
    this->species_id = safeStoi(data[2]);
    this->height = safeStoi(data[3]);
    this->weight = safeStoi(data[4]);
    this->base_experience = safeStoi(data[5]);
    this->order = safeStoi(data[6]);
    this->is_default = data[7] == "1";

    return *this;
  }
};

#endif