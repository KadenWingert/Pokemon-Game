#ifndef EXPERIENCE_H
#define EXPERIENCE_H

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class Experience {
 public:
  int growth_rate_id;
  int level;
  int experience;
  Experience& fromCSV(std::vector<std::string> data) {
    this->growth_rate_id = safeStoi(data[0]);
    this->level = safeStoi(data[1]);
    this->experience = safeStoi(data[2]);

    return *this;
  };
};

#endif