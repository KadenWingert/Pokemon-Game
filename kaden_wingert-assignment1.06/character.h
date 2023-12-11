#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>

# include "poke327.h"

//class Map;

//typedef int16_t pair_t[2];

class Character;


extern const char *char_type_name[num_character_types];

extern int32_t move_cost[num_character_types][num_terrain_types];

// typedef struct npc {
//   character_type_t ctype;
//   movement_type_t mtype;
//   int defeated;
//   pair_t dir;
// } npc_t;

// typedef struct pc {
// } pc_t;

/* character is defined in poke327.h to allow an instance of character
 * in world without including character.h in poke327.h                 */

int32_t cmp_char_turns(const void *key, const void *with);
uint32_t can_see(Map *m, Character *voyeur, Character *exhibitionist);
void delete_character(void *v);
void pathfind(Map *m);

extern void (*move_func[num_movement_types])(Character *, pair_t);

int pc_move(char);

#endif
