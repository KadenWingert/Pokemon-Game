#ifndef BATTLE_H
#define BATTLE_H

typedef class Trainer Trainer;
typedef class Game Game;
typedef class CohesivePokemon CohesivePokemon;

typedef enum { BATTLE_TYPE_TRAINER, BATTLE_TYPE_POKEMON } BattleType;

typedef enum {
  BATTLE_MENU_CHOOSE,
  BATTLE_MENU_FIGHT,
  BATTLE_MENU_POKEMON,
  BATTLE_MENU_BAG,
  BATTLE_MENU_RUN
} BattleMenu;

class Battle {
 public:
  Trainer* againstTrainer;
  CohesivePokemon* againstPokemon;
  CohesivePokemon* playerPokemon;
  BattleType type;
  BattleMenu subMenu;

  void startTrainerBattle(Trainer* against);
  void startPokemonBattle(CohesivePokemon* against);
};

#endif