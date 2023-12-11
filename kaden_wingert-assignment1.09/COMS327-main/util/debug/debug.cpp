#include "debug.h"

#include <fstream>
#include <iostream>

std::ofstream destination;

int isDebugging() { return destination.is_open(); }

void startDebugging() {
  if (isDebugging()) {
    return;
  }

  destination.open("debug.txt");
};

void debug(const char* message) {
  if (!isDebugging()) {
    return;
  }

  destination << message << std::endl;
  destination.flush();
}

void debug(std::string message) {
  if (!isDebugging()) {
    return;
  }

  destination << message << std::endl;
  destination.flush();
}

void debug(const char* identifier, int i) {
  if (!isDebugging()) {
    return;
  }

  destination << identifier << ": " << i << std::endl;
  destination.flush();
}

void debug(std::string identifier, int i) {
  if (!isDebugging()) {
    return;
  }

  destination << identifier << ": " << i << std::endl;
  destination.flush();
}

void debug(int i) {
  if (!isDebugging()) {
    return;
  }

  destination << i << std::endl;
  destination.flush();
}

void stopDebugging() {
  if (!isDebugging()) {
    return;
  }

  destination.close();
}