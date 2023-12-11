#include "gamedata.h"

#include "../../util/csv/csv.h"

void GameData::load() {
  this->loadExperience();
  this->loadMoves();
  this->loadStats();
  this->loadTypeNames();
  this->loadPokemonMoves();
  this->loadPokemonSpecies();
  this->loadPokemonStats();
  this->loadPokemonTypes();
  this->loadPokemon();
}

void GameData::loadMoves() {
  CSV* movesCSV = new CSV(this->sourceDirectory + "moves.csv");

  for (size_t i = 0; i < movesCSV->data.size(); i++) {
    Move move;
    move.fromCSV(movesCSV->data[i]);
    this->moves.insert(std::pair<int, Move>(move.id, move));
  }

  debug("Loaded " + std::to_string(movesCSV->data.size()) + " moves.");

  delete movesCSV;
}

void GameData::loadStats() {
  CSV* statsCSV = new CSV(this->sourceDirectory + "stats.csv");

  for (size_t i = 0; i < statsCSV->data.size(); i++) {
    Stat stat;
    stat.fromCSV(statsCSV->data[i]);
    this->stats.insert(std::pair<int, Stat>(stat.id, stat));
  }

  debug("Loaded " + std::to_string(statsCSV->data.size()) + " stats.");

  delete statsCSV;
}

void GameData::loadExperience() {
  CSV* experienceCSV = new CSV(this->sourceDirectory + "experience.csv");

  for (size_t i = 0; i < experienceCSV->data.size(); i++) {
    Experience experience;
    experience.fromCSV(experienceCSV->data[i]);
    this->experience.insert(
        std::pair<int, Experience>(experience.growth_rate_id, experience));
  }

  debug("Loaded " + std::to_string(experienceCSV->data.size()) +
        " experience entries.");

  delete experienceCSV;
}

void GameData::loadTypeNames() {
  CSV* typeNamesCSV = new CSV(this->sourceDirectory + "type_names.csv");

  for (size_t i = 0; i < typeNamesCSV->data.size(); i++) {
    TypeName typeName;
    typeName.fromCSV(typeNamesCSV->data[i]);

    if (typeName.local_language_id != 9) {
      continue;
    }

    this->typeNames.insert(
        std::pair<int, TypeName>(typeName.type_id, typeName));
  }

  debug("Loaded " + std::to_string(typeNamesCSV->data.size()) + " type names.");

  delete typeNamesCSV;
}

void GameData::loadPokemonMoves() {
  CSV* pokemonMovesCSV = new CSV(this->sourceDirectory + "pokemon_moves.csv");

  for (size_t i = 0; i < pokemonMovesCSV->data.size(); i++) {
    PokemonMove pokemonMove;
    pokemonMove.fromCSV(pokemonMovesCSV->data[i]);
    this->pokemonMoves.push_back(pokemonMove);
  }

  debug("Loaded " + std::to_string(pokemonMovesCSV->data.size()) +
        " pokemon moves.");

  delete pokemonMovesCSV;
}

void GameData::loadPokemonSpecies() {
  CSV* pokemonSpeciesCSV =
      new CSV(this->sourceDirectory + "pokemon_species.csv");

  for (size_t i = 0; i < pokemonSpeciesCSV->data.size(); i++) {
    PokemonSpecies pokemonSpecies;
    pokemonSpecies.fromCSV(pokemonSpeciesCSV->data[i]);
    this->pokemonSpecies.insert(
        std::pair<int, PokemonSpecies>(pokemonSpecies.id, pokemonSpecies));
  }

  debug("Loaded " + std::to_string(pokemonSpeciesCSV->data.size()) +
        " pokemon species.");

  delete pokemonSpeciesCSV;
}

void GameData::loadPokemonStats() {
  CSV* pokemonStatsCSV = new CSV(this->sourceDirectory + "pokemon_stats.csv");

  for (size_t i = 0; i < pokemonStatsCSV->data.size(); i++) {
    PokemonStat pokemonStat;
    pokemonStat.fromCSV(pokemonStatsCSV->data[i]);
    this->pokemonStats.push_back(pokemonStat);
  }

  debug("Loaded " + std::to_string(pokemonStatsCSV->data.size()) +
        " pokemon stats.");

  delete pokemonStatsCSV;
}

void GameData::loadPokemonTypes() {
  CSV* pokemonTypesCSV = new CSV(this->sourceDirectory + "pokemon_types.csv");

  for (size_t i = 0; i < pokemonTypesCSV->data.size(); i++) {
    PokemonType pokemonType;
    pokemonType.fromCSV(pokemonTypesCSV->data[i]);
    this->pokemonTypes.push_back(pokemonType);
  }

  debug("Loaded " + std::to_string(pokemonTypesCSV->data.size()) +
        " pokemon types.");

  delete pokemonTypesCSV;
}

void GameData::loadPokemon() {
  CSV* pokemonCSV = new CSV(this->sourceDirectory + "pokemon.csv");

  for (size_t i = 0; i < pokemonCSV->data.size(); i++) {
    Pokemon pokemon;
    pokemon.fromCSV(pokemonCSV->data[i]);
    this->pokemon.insert(std::pair<int, Pokemon>(pokemon.id, pokemon));
  }

  debug("Loaded " + std::to_string(pokemonCSV->data.size()) + " pokemon.");

  delete pokemonCSV;
}