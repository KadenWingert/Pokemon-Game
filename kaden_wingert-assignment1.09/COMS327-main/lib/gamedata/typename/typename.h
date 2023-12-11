#ifndef TYPE_NAMES_H
#define TYPE_NAMES_H

// type_id,local_language_id,name

#include <string>
#include <vector>

#include "../../../util/csv/csv.h"

class TypeName {
 public:
  int type_id;
  int local_language_id;
  std::string name;
  TypeName& fromCSV(std::vector<std::string> data) {
    this->type_id = safeStoi(data[0]);
    this->local_language_id = safeStoi(data[1]);
    this->name = data[2];

    return *this;
  };
};

#endif