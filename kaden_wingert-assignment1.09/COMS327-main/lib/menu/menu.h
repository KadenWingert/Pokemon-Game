#ifndef MENU_H
#define MENU_H

#define WINDOW_WIDTH 80
#define WINDOW_HEIGHT 24

#define MENU_WIDTH 72
#define MENU_HEIGHT 19

typedef class Game Game;

typedef enum MenuType {
  MENU_TYPE_TRAINER_LIST,
  MENU_TYPE_POKEMON_CENTER,
  MENU_TYPE_POKEMART,
  MENU_TYPE_BATTLE,
  MENU_TYPE_CHOOSE_STARTER,
  MENU_TYPE_BAG
} MenuType;

class Menu {
 public:
  MenuType type;
  int position;
  int (*move)(int move, Game *game);
  void (*render)(Game *game);
};

extern const char *menuHeaders[];

int prepareMenu(MenuType type, Menu *menu);
int destroyMenu(Menu *menu);

#endif