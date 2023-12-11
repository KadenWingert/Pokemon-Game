#ifndef DEBUG_H
#define DEBUG_H

#include <string>

void startDebugging();
void debug(const char* message);
void debug(std::string message);
void debug(const char* identifier, int i);
void debug(std::string identifier, int i);
void debug(int i);
void stopDebugging();
int isDebugging();

#endif