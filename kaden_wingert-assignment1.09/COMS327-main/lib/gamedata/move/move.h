#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class Move {
 public:
  int id;
  std::string identifier;
  int generation_id;
  int type_id;
  int power;
  int pp;
  int accuracy;
  int priority;
  int target_id;
  int damage_class_id;
  int effect_id;
  int effect_chance;
  int contest_type_id;
  int contest_effect_id;
  int super_contest_effect_id;
  Move& fromCSV(std::vector<std::string> data) {
    this->id = safeStoi(data[0]);
    this->identifier = data[1];
    this->generation_id = safeStoi(data[2]);
    this->type_id = safeStoi(data[3]);
    this->power = safeStoi(data[4]);
    this->pp = safeStoi(data[5]);
    this->accuracy = safeStoi(data[6]);
    this->priority = safeStoi(data[7]);
    this->target_id = safeStoi(data[8]);
    this->damage_class_id = safeStoi(data[9]);
    this->effect_id = safeStoi(data[10]);
    this->effect_chance = safeStoi(data[11]);
    this->contest_type_id = safeStoi(data[12]);
    this->contest_effect_id = safeStoi(data[13]);
    this->super_contest_effect_id = safeStoi(data[14]);

    return *this;
  };
};

#endif