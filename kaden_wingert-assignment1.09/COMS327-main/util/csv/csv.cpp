#include "csv.h"

void CSV::parseContent() {
  std::string line;

  bool firstLine = true;

  while (std::getline(this->fileStream, line)) {
    std::vector<std::string> row;
    std::string cell = "";

    for (size_t i = 0; i < line.size(); i++) {
      if (line[i] == ',') {
        row.push_back(cell);
        cell = "";
      } else {
        cell += line[i];
      }
    }

    row.push_back(cell);

    if (firstLine) {
      this->header = row;
      firstLine = false;
    } else {
      this->data.push_back(row);
    }
  }

  this->fileStream.close();
}

int safeStoi(std::string str) {
  try {
    return std::stoi(str);
  } catch (std::invalid_argument& e) {
    return __INT_MAX__;
  }
}