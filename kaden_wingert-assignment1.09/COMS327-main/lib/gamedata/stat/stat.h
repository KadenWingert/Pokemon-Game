#ifndef STAT_H
#define STAT_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class Stat {
 public:
  int id;
  int damage_class_id;
  std::string identifier;
  bool is_battle_only;
  int game_index;
  Stat& fromCSV(std::vector<std::string> data) {
    this->id = safeStoi(data[0]);
    this->damage_class_id = safeStoi(data[1]);
    this->identifier = data[2];
    this->is_battle_only = data[3] == "1";
    this->game_index = safeStoi(data[4]);

    return *this;
  };
};

#endif