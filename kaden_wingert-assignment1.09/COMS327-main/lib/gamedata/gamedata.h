#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <sys/stat.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../../util/debug/debug.h"
#include "./experience/experience.h"
#include "./move/move.h"
#include "./pokemon/pokemon.h"
#include "./pokemonmove/pokemonmove.h"
#include "./pokemonspecies/pokemonspecies.h"
#include "./pokemonstat/pokemonstat.h"
#include "./pokemontype/pokemontype.h"
#include "./stat/stat.h"
#include "./typename/typename.h"

class GameData {
 private:
  std::string sourceDirectory;

 public:
  void load();
  void loadMoves();
  void loadStats();
  void loadExperience();
  void loadTypeNames();
  void loadPokemonMoves();
  void loadPokemonSpecies();
  void loadPokemonStats();
  void loadPokemonTypes();
  void loadPokemon();

  std::unordered_map<int, Move> moves;
  std::unordered_map<int, Stat> stats;
  std::unordered_map<int, Experience> experience;
  std::unordered_map<int, TypeName> typeNames;
  std::vector<PokemonMove> pokemonMoves;
  std::unordered_map<int, PokemonSpecies> pokemonSpecies;
  std::vector<PokemonStat> pokemonStats;
  std::vector<PokemonType> pokemonTypes;
  std::unordered_map<int, Pokemon> pokemon;

  GameData() {
    struct stat buffer;

    if (stat("/share/cs327/", &buffer) == 0) {
      this->sourceDirectory = "/share/cs327/pokedex/pokedex/data/csv/";
    } else if (stat((std::string(std::getenv("HOME")) + "/.poke327/").c_str(),
                    &buffer) == 0) {
      this->sourceDirectory = std::string(std::getenv("HOME")) + "/.poke327/";
    } else if (stat("../data", &buffer) == 0) {
      this->sourceDirectory = "../data/";
    }

    if (this->sourceDirectory == "") {
      throw std::runtime_error("Could not find game data directory!");
    }

    debug("Using data directory: " + this->sourceDirectory);

    load();
  }
};

extern GameData* gameData;

#endif