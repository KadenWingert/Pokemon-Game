#ifndef POKEMON_SPECIES_H
#define POKEMON_SPECIES_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class PokemonSpecies {
 public:
  int id;
  std::string identifier;
  int generation_id;
  int evolves_from_species_id;
  int evolution_chain_id;
  int color_id;
  int shape_id;
  int habitat_id;
  int gender_rate;
  int capture_rate;
  int base_happiness;
  bool is_baby;
  int hatch_counter;
  bool has_gender_differences;
  int growth_rate_id;
  bool forms_switchable;
  bool is_legendary;
  bool is_mythical;
  int order;
  int conquest_order;

  PokemonSpecies& fromCSV(std::vector<std::string> data) {
    this->id = safeStoi(data[0]);
    this->identifier = data[1];
    this->generation_id = safeStoi(data[2]);
    this->evolves_from_species_id = safeStoi(data[3]);
    this->evolution_chain_id = safeStoi(data[4]);
    this->color_id = safeStoi(data[5]);
    this->shape_id = safeStoi(data[6]);
    this->habitat_id = safeStoi(data[7]);
    this->gender_rate = safeStoi(data[8]);
    this->capture_rate = safeStoi(data[9]);
    this->base_happiness = safeStoi(data[10]);
    this->is_baby = data[11] == "1";
    this->hatch_counter = safeStoi(data[12]);
    this->has_gender_differences = data[13] == "1";
    this->growth_rate_id = safeStoi(data[14]);
    this->forms_switchable = data[15] == "1";
    this->is_legendary = data[16] == "1";
    this->is_mythical = data[17] == "1";
    this->order = safeStoi(data[18]);
    this->conquest_order = safeStoi(data[19]);

    return *this;
  }
};

#endif