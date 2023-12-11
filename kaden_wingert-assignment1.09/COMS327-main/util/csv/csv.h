#ifndef CSV_H
#define CSV_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class CSV {
 private:
  std::ifstream fileStream;
  void parseContent();

 public:
  std::vector<std::string> header;
  std::vector<std::vector<std::string> > data;
  CSV(std::string fileName) : fileStream(fileName) {
    if (!this->fileStream.is_open()) {
      throw std::runtime_error("Error opening file: " + fileName);
    }

    this->parseContent();
  }
};

int safeStoi(std::string str);

#endif