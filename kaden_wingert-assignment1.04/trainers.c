#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "heap.h"
#include <stdbool.h>
#define MAP_X 80
#define MAP_Y 21

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

typedef struct map_differentPaths
{
  heap_node_t *hn_character;
  uint8_t position[2];
  uint8_t from[2];
  int32_t cost;
} map_differentPaths_t;

typedef enum dimension
{
  dimension_x,
  dimension_y,
  numberOfDimensions
} dimension_t;

typedef int16_t pair_t[numberOfDimensions];
#define map_Pair(pair) (m->map[pair[dimension_y]][pair[dimension_x]])
#define mapxy(x, y) (m->map[y][x])
#define height_characters(pair) (m->height[pair[dimension_y]][pair[dimension_x]])
#define height_coordinates(x, y) (m->height[y][x])

typedef enum __attribute__((__packed__)) terrain_type
{
  terrain_boulder,
  terrain_trees,
  terrain_mapPath,
  terrain_mart,
  terrain_center,
  terrain_grass,
  terrain_clearing,
  terrain_mountains,
  terrain_forest,
  terrain_water, // TODO
  terrain_gate,
  terrain_bridge,
  numberOfDifferentTerrains
} terrain_type_t;

typedef enum __attribute__((__packed__)) different_characters
{
  char_playercharacter,
  character_hiker,
  character_rival,
  character_swimmer, // TODO
  character_other,
  character_pacer,
  character_wanderer,
  character_stationary,
  character_explorer,
  countDifferentCharacters
} different_characters_t;

typedef struct playerCharacter
{
  pair_t position;
} playerCharacter_t;

typedef struct trainer
{
  heap_node_t *hn_character;
  int position[2];
  int next_position[2];
  different_characters_t type;
  char direction;
  terrain_type_t ter;
  int cost;
  bool display;
} trainer_t;

typedef struct map
{
  int8_t n, s, e, w;
  terrain_type_t map[21][80];
  uint8_t height[21][80];
} map_t;

typedef struct queue_node
{
  int x, y;
  struct queue_node *next;
} queue_node_t;

typedef struct world
{
  map_t *world[401][401];
  pair_t currator_idk;
  map_t *cur_idx;
  int hiker_dist[21][80];
  int rival_dist[21][80];
  int swimmer_dist[21][80];
  trainer_t trainers[21][80];
  playerCharacter_t playerCharacter;
} world_t;
world_t world;
static int32_t cost_DifferentMoves[countDifferentCharacters][numberOfDifferentTerrains];
static int32_t map_differentPaths_cmp(const void *key, const void *with);
static int32_t findEdgePenalty(int8_t x, int8_t y);
static void dijkstra_map_differentPaths(map_t *m, pair_t from, pair_t to);
static int build_map_differentPathss(map_t *m);
static int gaussian[5][5];
static int smooth_height(map_t *m);
static void findLocationBuild(map_t *m, pair_t p);
static int place_pokemart(map_t *m);
static int place_center(map_t *m);
static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w);
static int place_boulders(map_t *m);
static int place_trees(map_t *m);
static int new_map();
static void print_map();
void init_world();
void delete_world();
static int32_t hiker_characterMainPlayer(const void *key, const void *with);
static int32_t swimmer_characterMainPlayer(const void *key, const void *with);
static int32_t rival_characterMainPlayer(const void *key, const void *with);
static int32_t trainer_characterMainPlayer(const void *key, const void *with);
void map_findDifferentPaths(map_t *m);
void init_differentPlayerCharacters();
void initializeTrainers(uint16_t number_DifferentTrainers, map_t *m);
void set_LogicsNext(trainer_t *t, map_t *m);
void printHikerDistance();
void print_rivalCharacter();
void showTrainers();
static terrain_type_t border_type(map_t *m, int32_t x, int32_t y);

int main(int argc, char *argv[])
{
  struct timeval tv;
  uint32_t seed;
  uint32_t number_DifferentTrainers = 10;
  char c;
  int x, y;
  // Accepts a switch to add more trainers
  if (argc == 2)
  {
    seed = atoi(argv[1]);
  }
  else
  {
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }
  if (argc > 2)
  {
    if (!strcmp(argv[1], "--numtrainers"))
    {
      if (argc == 3)
      {
        number_DifferentTrainers = atoi(argv[2]);
      }
    }
    else if (!strcmp(argv[2], "--numtrainers"))
    {
      if (argc == 4)
      {
        number_DifferentTrainers = atoi(argv[3]);
      }
    }
    else
    {
      printf("Invalid switch... using Defaults\n");
    }
  }

  printf("Using %d Trainers\n", number_DifferentTrainers);

  printf("Using seed: %u\n", seed);
  srand(seed);

  init_world();
  init_differentPlayerCharacters();
  map_findDifferentPaths(world.cur_idx);
  initializeTrainers(number_DifferentTrainers, world.cur_idx);

  heap_t h;
  heap_init(&h, trainer_characterMainPlayer, NULL);

  for (y = 1; y < 21 - 1; y++)
  {
    for (x = 1; x < 80 - 1; x++)
    {
      if (world.trainers[y][x].display && world.trainers[y][x].type != character_stationary)
      {
        heap_insert(&h, &world.trainers[y][x]);
      }
    }
  }
  while (1)
  {
    trainer_t *t;
    usleep(100000);
    if (h.size > 0)
    {
      t = heap_remove_min(&h);
    }
    else
    {
      break;
    }
    print_map();
    int temp_x = t->position[0];
    int temp_y = t->position[1];
    set_LogicsNext(t, world.cur_idx);
    world.trainers[t->next_position[1]][t->next_position[0]].direction = t->direction;
    world.trainers[t->next_position[1]][t->next_position[0]].cost = t->cost;
    world.trainers[t->next_position[1]][t->next_position[0]].ter = t->ter;
    world.trainers[t->next_position[1]][t->next_position[0]].position[0] = t->position[0];
    world.trainers[t->next_position[1]][t->next_position[0]].position[1] = t->position[1];
    world.trainers[t->next_position[1]][t->next_position[0]].type = t->type;
    world.trainers[t->next_position[1]][t->next_position[0]].next_position[0] = t->position[0];
    world.trainers[t->next_position[1]][t->next_position[0]].next_position[1] = t->position[1];
    world.trainers[t->next_position[1]][t->next_position[0]].display = true;
    t->position[0] = t->next_position[0];
    t->position[1] = t->next_position[1];
    // if (t->cost != 0 && (temp_x != t->position[0] || temp_y != t->position[1]))
    if (temp_x != t->position[0] || temp_y != t->position[1]) // If they have moved
      world.trainers[temp_y][temp_x].display = false;         // Don't show the old location before they moved

    heap_insert(&h, t);
  }
  return 0;

  do
  {
    print_map();
    printf("Current position is %d%cx%d%c (%d,%d).  "
           "Enter command: ",
           abs(world.currator_idk[dimension_x] - (401 / 2)),
           world.currator_idk[dimension_x] - (401 / 2) >= 0 ? 'E' : 'W',
           abs(world.currator_idk[dimension_y] - (401 / 2)),
           world.currator_idk[dimension_y] - (401 / 2) <= 0 ? 'N' : 'S',
           world.currator_idk[dimension_x] - (401 / 2),
           world.currator_idk[dimension_y] - (401 / 2));
    scanf(" %c", &c);
    switch (c)
    {
    case 'n':
      if (world.currator_idk[dimension_y])
      {
        world.currator_idk[dimension_y]--;
        new_map();
      }
      break;
    case 's':
      if (world.currator_idk[dimension_y] < 401 - 1)
      {
        world.currator_idk[dimension_y]++;
        new_map();
      }
      break;
    case 'w':
      if (world.currator_idk[dimension_x])
      {
        world.currator_idk[dimension_x]--;
        new_map();
      }
      break;
    case 'e':
      if (world.currator_idk[dimension_x] < 401 - 1)
      {
        world.currator_idk[dimension_x]++;
        new_map();
      }
      break;
    case 'q':
      break;
    case 'f':
      scanf(" %d %d", &x, &y);
      if (x >= -(401 / 2) && x <= 401 / 2 &&
          y >= -(401 / 2) && y <= 401 / 2)
      {
        world.currator_idk[dimension_x] = x + (401 / 2);
        world.currator_idk[dimension_y] = y + (401 / 2);
        new_map();
      }
      break;
    case '?':
    case 'h':
      printf("Write 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
             "Quit 'q'.  '?' and 'h' print help message.\n");
      break;
    default:
      fprintf(stderr, "%c: Invalid input.\n", c);
      break;
    }
  } while (c != 'q');

  delete_world();

  return 0;
}

#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

static int32_t cost_DifferentMoves[countDifferentCharacters][numberOfDifferentTerrains] = {
    {INT_MAX, INT_MAX, 10, 10, 10, 20, 10, INT_MAX, INT_MAX, INT_MAX, 10, 10},
    {INT_MAX, INT_MAX, 10, 50, 50, 15, 10, 15, 15, INT_MAX, INT_MAX, 10},
    {INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 10},
    {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 7, INT_MAX, 7},
    {INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 10},
};

static int32_t findEdgePenalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == 80 - 2 || y == 21 - 2) ? 2 : 1;
}

static void dijkstra_map_differentPaths(map_t *m, pair_t from, pair_t to)
{
  static map_differentPaths_t map_differentPaths[21][80], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized)
  {
    for (y = 0; y < 21; y++)
    {
      for (x = 0; x < 80; x++)
      {
        map_differentPaths[y][x].position[dimension_y] = y;
        map_differentPaths[y][x].position[dimension_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      map_differentPaths[y][x].cost = INT_MAX;
    }
  }

  map_differentPaths[from[dimension_y]][from[dimension_x]].cost = 0;

  heap_init(&h, map_differentPaths_cmp, NULL);

  for (y = 1; y < 21 - 1; y++)
  {
    for (x = 1; x < 80 - 1; x++)
    {
      map_differentPaths[y][x].hn_character = heap_insert(&h, &map_differentPaths[y][x]);
    }
  }

  while ((p = heap_remove_min(&h)))
  {
    p->hn_character = NULL;

    if ((p->position[dimension_y] == to[dimension_y]) && p->position[dimension_x] == to[dimension_x])
    {
      for (x = to[dimension_x], y = to[dimension_y];
           (x != from[dimension_x]) || (y != from[dimension_y]);
           p = &map_differentPaths[y][x], x = p->from[dimension_x], y = p->from[dimension_y])
      {
        mapxy(x, y) = terrain_mapPath;
        height_coordinates(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((map_differentPaths[p->position[dimension_y] - 1][p->position[dimension_x]].hn_character) &&
        (map_differentPaths[p->position[dimension_y] - 1][p->position[dimension_x]].cost >
         ((p->cost + height_characters(p->position)) *
          findEdgePenalty(p->position[dimension_x], p->position[dimension_y] - 1))))
    {
      map_differentPaths[p->position[dimension_y] - 1][p->position[dimension_x]].cost =
          ((p->cost + height_characters(p->position)) *
           findEdgePenalty(p->position[dimension_x], p->position[dimension_y] - 1));
      map_differentPaths[p->position[dimension_y] - 1][p->position[dimension_x]].from[dimension_y] = p->position[dimension_y];
      map_differentPaths[p->position[dimension_y] - 1][p->position[dimension_x]].from[dimension_x] = p->position[dimension_x];
      heap_decrease_key_no_replace(&h, map_differentPaths[p->position[dimension_y] - 1]
                                                         [p->position[dimension_x]]
                                                             .hn_character);
    }
    if ((map_differentPaths[p->position[dimension_y]][p->position[dimension_x] - 1].hn_character) &&
        (map_differentPaths[p->position[dimension_y]][p->position[dimension_x] - 1].cost >
         ((p->cost + height_characters(p->position)) *
          findEdgePenalty(p->position[dimension_x] - 1, p->position[dimension_y]))))
    {
      map_differentPaths[p->position[dimension_y]][p->position[dimension_x] - 1].cost =
          ((p->cost + height_characters(p->position)) *
           findEdgePenalty(p->position[dimension_x] - 1, p->position[dimension_y]));
      map_differentPaths[p->position[dimension_y]][p->position[dimension_x] - 1].from[dimension_y] = p->position[dimension_y];
      map_differentPaths[p->position[dimension_y]][p->position[dimension_x] - 1].from[dimension_x] = p->position[dimension_x];
      heap_decrease_key_no_replace(&h, map_differentPaths[p->position[dimension_y]]
                                                         [p->position[dimension_x] - 1]
                                                             .hn_character);
    }
    if ((map_differentPaths[p->position[dimension_y]][p->position[dimension_x] + 1].hn_character) &&
        (map_differentPaths[p->position[dimension_y]][p->position[dimension_x] + 1].cost >
         ((p->cost + height_characters(p->position)) *
          findEdgePenalty(p->position[dimension_x] + 1, p->position[dimension_y]))))
    {
      map_differentPaths[p->position[dimension_y]][p->position[dimension_x] + 1].cost =
          ((p->cost + height_characters(p->position)) *
           findEdgePenalty(p->position[dimension_x] + 1, p->position[dimension_y]));
      map_differentPaths[p->position[dimension_y]][p->position[dimension_x] + 1].from[dimension_y] = p->position[dimension_y];
      map_differentPaths[p->position[dimension_y]][p->position[dimension_x] + 1].from[dimension_x] = p->position[dimension_x];
      heap_decrease_key_no_replace(&h, map_differentPaths[p->position[dimension_y]]
                                                         [p->position[dimension_x] + 1]
                                                             .hn_character);
    }
    if ((map_differentPaths[p->position[dimension_y] + 1][p->position[dimension_x]].hn_character) &&
        (map_differentPaths[p->position[dimension_y] + 1][p->position[dimension_x]].cost >
         ((p->cost + height_characters(p->position)) *
          findEdgePenalty(p->position[dimension_x], p->position[dimension_y] + 1))))
    {
      map_differentPaths[p->position[dimension_y] + 1][p->position[dimension_x]].cost =
          ((p->cost + height_characters(p->position)) *
           findEdgePenalty(p->position[dimension_x], p->position[dimension_y] + 1));
      map_differentPaths[p->position[dimension_y] + 1][p->position[dimension_x]].from[dimension_y] = p->position[dimension_y];
      map_differentPaths[p->position[dimension_y] + 1][p->position[dimension_x]].from[dimension_x] = p->position[dimension_x];
      heap_decrease_key_no_replace(&h, map_differentPaths[p->position[dimension_y] + 1]
                                                         [p->position[dimension_x]]
                                                             .hn_character);
    }
  }
}

static int build_map_differentPathss(map_t *m)
{
  pair_t from, to;

  if (m->e != -1 && m->w != -1)
  {
    from[dimension_x] = 1;
    to[dimension_x] = 80 - 2;
    from[dimension_y] = m->w;
    to[dimension_y] = m->e;

    dijkstra_map_differentPaths(m, from, to);
  }

  if (m->n != -1 && m->s != -1)
  {
    from[dimension_y] = 1;
    to[dimension_y] = 21 - 2;
    from[dimension_x] = m->n;
    to[dimension_x] = m->s;

    dijkstra_map_differentPaths(m, from, to);
  }

  if (m->e == -1)
  {
    if (m->s == -1)
    {
      from[dimension_x] = 1;
      from[dimension_y] = m->w;
      to[dimension_x] = m->n;
      to[dimension_y] = 1;
    }
    else
    {
      from[dimension_x] = 1;
      from[dimension_y] = m->w;
      to[dimension_x] = m->s;
      to[dimension_y] = 21 - 2;
    }

    dijkstra_map_differentPaths(m, from, to);
  }

  if (m->w == -1)
  {
    if (m->s == -1)
    {
      from[dimension_x] = 80 - 2;
      from[dimension_y] = m->e;
      to[dimension_x] = m->n;
      to[dimension_y] = 1;
    }
    else
    {
      from[dimension_x] = 80 - 2;
      from[dimension_y] = m->e;
      to[dimension_x] = m->s;
      to[dimension_y] = 21 - 2;
    }

    dijkstra_map_differentPaths(m, from, to);
  }

  if (m->n == -1)
  {
    if (m->e == -1)
    {
      from[dimension_x] = 1;
      from[dimension_y] = m->w;
      to[dimension_x] = m->s;
      to[dimension_y] = 21 - 2;
    }
    else
    {
      from[dimension_x] = 80 - 2;
      from[dimension_y] = m->e;
      to[dimension_x] = m->s;
      to[dimension_y] = 21 - 2;
    }

    dijkstra_map_differentPaths(m, from, to);
  }

  if (m->s == -1)
  {
    if (m->e == -1)
    {
      from[dimension_x] = 1;
      from[dimension_y] = m->w;
      to[dimension_x] = m->n;
      to[dimension_y] = 1;
    }
    else
    {
      from[dimension_x] = 80 - 2;
      from[dimension_y] = m->e;
      to[dimension_x] = m->n;
      to[dimension_y] = 1;
    }

    dijkstra_map_differentPaths(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
    {1, 4, 7, 4, 1},
    {4, 16, 26, 16, 4},
    {7, 26, 41, 26, 7},
    {4, 16, 26, 16, 4},
    {1, 4, 7, 4, 1}};

static int32_t map_differentPaths_cmp(const void *key, const void *with)
{
  return ((map_differentPaths_t *)key)->cost - ((map_differentPaths_t *)with)->cost;
}

static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  uint8_t height[21][80];

  memset(&height, 0, sizeof(height));

  for (i = 1; i < 255; i += 20)
  {
    do
    {
      x = rand() % 80;
      y = rand() % 21;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1)
    {
      head = tail = malloc(sizeof(*tail));
    }
    else
    {
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }
  while (head)
  {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1])
    {
      height[y - 1][x - 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1])
    {
      height[y][x - 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < 21 && !height[y + 1][x - 1])
    {
      height[y + 1][x - 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x])
    {
      height[y - 1][x] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < 21 && !height[y + 1][x])
    {
      height[y + 1][x] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < 80 && y - 1 >= 0 && !height[y - 1][x + 1])
    {
      height[y - 1][x + 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < 80 && !height[y][x + 1])
    {
      height[y][x + 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < 80 && y + 1 < 21 && !height[y + 1][x + 1])
    {
      height[y + 1][x + 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      for (s = t = p = 0; p < 5; p++)
      {
        for (q = 0; q < 5; q++)
        {
          if (y + (p - 2) >= 0 && y + (p - 2) < 21 &&
              x + (q - 2) >= 0 && x + (q - 2) < 80)
          {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      for (s = t = p = 0; p < 5; p++)
      {
        for (q = 0; q < 5; q++)
        {
          if (y + (p - 2) >= 0 && y + (p - 2) < 21 &&
              x + (q - 2) >= 0 && x + (q - 2) < 80)
          {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  return 0;
}

static void findLocationBuild(map_t *m, pair_t p)
{
  do
  {
    p[dimension_x] = rand() % (80 - 5) + 3;
    p[dimension_y] = rand() % (21 - 10) + 5;

    if ((((mapxy(p[dimension_x] - 1, p[dimension_y]) == terrain_mapPath) &&
          (mapxy(p[dimension_x] - 1, p[dimension_y] + 1) == terrain_mapPath)) ||
         ((mapxy(p[dimension_x] + 2, p[dimension_y]) == terrain_mapPath) &&
          (mapxy(p[dimension_x] + 2, p[dimension_y] + 1) == terrain_mapPath)) ||
         ((mapxy(p[dimension_x], p[dimension_y] - 1) == terrain_mapPath) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y] - 1) == terrain_mapPath)) ||
         ((mapxy(p[dimension_x], p[dimension_y] + 2) == terrain_mapPath) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y] + 2) == terrain_mapPath))) &&
        (((mapxy(p[dimension_x], p[dimension_y]) != terrain_mart) &&
          (mapxy(p[dimension_x], p[dimension_y]) != terrain_center) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y]) != terrain_mart) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y]) != terrain_center) &&
          (mapxy(p[dimension_x], p[dimension_y] + 1) != terrain_mart) &&
          (mapxy(p[dimension_x], p[dimension_y] + 1) != terrain_center) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y] + 1) != terrain_mart) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y] + 1) != terrain_center))) &&
        (((mapxy(p[dimension_x], p[dimension_y]) != terrain_mapPath) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y]) != terrain_mapPath) &&
          (mapxy(p[dimension_x], p[dimension_y] + 1) != terrain_mapPath) &&
          (mapxy(p[dimension_x] + 1, p[dimension_y] + 1) != terrain_mapPath))))
    {
      break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  findLocationBuild(m, p);

  mapxy(p[dimension_x], p[dimension_y]) = terrain_mart;
  mapxy(p[dimension_x] + 1, p[dimension_y]) = terrain_mart;
  mapxy(p[dimension_x], p[dimension_y] + 1) = terrain_mart;
  mapxy(p[dimension_x] + 1, p[dimension_y] + 1) = terrain_mart;

  return 0;
}

static int place_center(map_t *m)
{
  pair_t p;

  findLocationBuild(m, p);

  mapxy(p[dimension_x], p[dimension_y]) = terrain_center;
  mapxy(p[dimension_x] + 1, p[dimension_y]) = terrain_center;
  mapxy(p[dimension_x], p[dimension_y] + 1) = terrain_center;
  mapxy(p[dimension_x] + 1, p[dimension_y] + 1) = terrain_center;

  return 0;
}
/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(map_t *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;

  r = t = 0;

  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1 : MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1 : MAP_X;

  for (q = miny; q < maxy; q++)
  {
    for (p = minx; p < maxx; p++)
    {
      if (q != y || p != x)
      {
        if (m->map[q][p] == terrain_mountains ||
            m->map[q][p] == terrain_boulder)
        {
          r++;
        }
        else if (m->map[q][p] == terrain_forest ||
                 m->map[q][p] == terrain_trees)
        {
          t++;
        }
      }
    }
  }

  if (t == r)
  {
    return rand() & 1 ? terrain_boulder : terrain_trees;
  }
  else if (t > r)
  {
    if (rand() % 10)
    {
      return terrain_trees;
    }
    else
    {
      return terrain_boulder;
    }
  }
  else
  {
    if (rand() % 10)
    {
      return terrain_boulder;
    }
    else
    {
      return terrain_trees;
    }
  }
}

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;

  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof(m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++)
  {
    do
    {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0)
    {
      type = terrain_grass;
    }
    else if (i == num_grass)
    {
      type = terrain_clearing;
    }
    else if (i == num_grass + num_clearing)
    {
      type = terrain_mountains;
    }
    else if (i == num_grass + num_clearing + num_mountain)
    {
      type = terrain_forest;
    }
    else if (i == num_grass + num_clearing + num_mountain + num_forest)
    {
      type = terrain_water;
    }
    m->map[y][x] = type;
    if (i == 0)
    {
      head = tail = malloc(sizeof(*tail));
    }
    else
    {
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /* Diffuse the vaules to fill the space */
  while (head)
  {
    x = head->x;
    y = head->y;
    i = m->map[y][x];

    if (x - 1 >= 0 && !m->map[y][x - 1])
    {
      if ((rand() % 100) < 80)
      {
        m->map[y][x - 1] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x])
    {
      if ((rand() % 100) < 20)
      {
        m->map[y - 1][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x])
    {
      if ((rand() % 100) < 20)
      {
        m->map[y + 1][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1])
    {
      if ((rand() % 100) < 80)
      {
        m->map[y][x + 1] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1)
      {
        mapxy(x, y) = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1)
  {
    mapxy(n, 0) = terrain_gate;
    mapxy(n, 1) = terrain_gate;
  }
  if (s != -1)
  {
    mapxy(s, MAP_Y - 1) = terrain_gate;
    mapxy(s, MAP_Y - 2) = terrain_gate;
  }
  if (w != -1)
  {
    mapxy(0, w) = terrain_gate;
    mapxy(1, w) = terrain_gate;
  }
  if (e != -1)
  {
    mapxy(MAP_X - 1, e) = terrain_gate;
    mapxy(MAP_X - 2, e) = terrain_gate;
  }

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < 10 || rand() % 100 < 95; i++)
  {
    y = rand() % (21 - 2) + 1;
    x = rand() % (80 - 2) + 1;
    if (m->map[y][x] != terrain_forest && m->map[y][x] != terrain_mapPath)
    {
      m->map[y][x] = terrain_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < 10 || rand() % 100 < 95; i++)
  {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != terrain_mountains &&
        m->map[y][x] != terrain_mapPath &&
        m->map[y][x] != terrain_water)
    {
      m->map[y][x] = terrain_trees;
    }
  }

  return 0;
}
static int new_map()
{
  int d, p;
  int e, w, n, s;

  if (world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x]])
  {
    world.cur_idx = world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x]];
    return 0;
  }

  world.cur_idx =
      world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x]] =
          malloc(sizeof(*world.cur_idx));

  smooth_height(world.cur_idx);

  if (!world.currator_idk[dimension_y])
  {
    n = -1;
  }
  else if (world.world[world.currator_idk[dimension_y] - 1][world.currator_idk[dimension_x]])
  {
    n = world.world[world.currator_idk[dimension_y] - 1][world.currator_idk[dimension_x]]->s;
  }
  else
  {
    n = 1 + rand() % (80 - 2);
  }
  if (world.currator_idk[dimension_y] == 401 - 1)
  {
    s = -1;
  }
  else if (world.world[world.currator_idk[dimension_y] + 1][world.currator_idk[dimension_x]])
  {
    s = world.world[world.currator_idk[dimension_y] + 1][world.currator_idk[dimension_x]]->n;
  }
  else
  {
    s = 1 + rand() % (80 - 2);
  }
  if (!world.currator_idk[dimension_x])
  {
    w = -1;
  }
  else if (world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x] - 1])
  {
    w = world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x] - 1]->e;
  }
  else
  {
    w = 1 + rand() % (21 - 2);
  }
  if (world.currator_idk[dimension_x] == 401 - 1)
  {
    e = -1;
  }
  else if (world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x] + 1])
  {
    e = world.world[world.currator_idk[dimension_y]][world.currator_idk[dimension_x] + 1]->w;
  }
  else
  {
    e = 1 + rand() % (21 - 2);
  }

  map_terrain(world.cur_idx, n, s, e, w);

  place_boulders(world.cur_idx);
  place_trees(world.cur_idx);
  build_map_differentPathss(world.cur_idx);

  // checks if a path is a bridge
  for (int i = 0; i < 21; ++i)
  {
    for (int j = 0; j < 80; ++j)
    {
      if (world.cur_idx->map[i][j] == terrain_mapPath)
      { // If it is a path

        if (world.cur_idx->map[i + 1][j] == terrain_water) // If any of the 4 cardinal directions next to it are water, then this spot becomes a bridge
          world.cur_idx->map[i][j] = terrain_bridge;
        if (world.cur_idx->map[i - 1][j] == terrain_water)
          world.cur_idx->map[i][j] = terrain_bridge;
        if (world.cur_idx->map[i][j + 1] == terrain_water)
          world.cur_idx->map[i][j] = terrain_bridge;
        if (world.cur_idx->map[i][j - 1] == terrain_water)
          world.cur_idx->map[i][j] = terrain_bridge;
      }
    }
  }


  d = (abs(world.currator_idk[dimension_x] - (401 / 2)) +
       abs(world.currator_idk[dimension_y] - (401 / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  if ((rand() % 100) < p || !d)
  {
    place_pokemart(world.cur_idx);
  }
  if ((rand() % 100) < p || !d)
  {
    place_center(world.cur_idx);
  }

  return 0;
}

static void print_map()
{
  int x, y;
  int default_reached = 0;

  printf("\n\n\n");

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      if (world.playerCharacter.position[dimension_x] == x &&
          world.playerCharacter.position[dimension_y] == y)
      {
        putchar('@');
      }
      else if (world.trainers[y][x].display)
      {
        if (world.trainers[y][x].type == character_hiker)
        {
          putchar('h');
        }
        else if (world.trainers[y][x].type == character_rival)
        {
          putchar('r');
        }
        else if (world.trainers[y][x].type == character_pacer)
        {
          putchar('p');
        }
        else if (world.trainers[y][x].type == character_stationary)
        {
          putchar('s');
        }
        else if (world.trainers[y][x].type == character_wanderer)
        {
          putchar('w');
        }
        else if (world.trainers[y][x].type == character_swimmer)
        {
          putchar('m');
        }
        else if (world.trainers[y][x].type == character_explorer)
        {
          putchar('e');
        }
      }
      else
      {
        switch (world.cur_idx->map[y][x])
        {
        case terrain_boulder:
        case terrain_mountains:
          putchar('%');
          break;
        case terrain_trees:
        case terrain_forest:
          putchar('^');
          break;
        case terrain_center:
          putchar('C');
          break;
        case terrain_mapPath:
        case terrain_gate:
        case terrain_bridge:
          putchar('#');
          break;
        case terrain_mart:
          putchar('M');
          break;
        case terrain_grass:
          putchar(':');
          break;
        case terrain_water: // TODO
          putchar('~');
          break;
        case terrain_clearing:
          putchar('.');
          break;
        default:
          default_reached = 1;
          break;
        }
      }
    }
    putchar('\n');
  }

  if (default_reached)
  {
    fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
  }
}

void init_world()
{
  world.currator_idk[dimension_x] = world.currator_idk[dimension_y] = 401 / 2;
  new_map();
}

void delete_world()
{
  int x, y;

  for (y = 0; y < 401; y++)
  {
    for (x = 0; x < 401; x++)
    {
      if (world.world[y][x])
      {
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

#define ter_cost(x, y, c) cost_DifferentMoves[c][m->map[y][x]]

static int32_t hiker_characterMainPlayer(const void *key, const void *with)
{
  return (world.hiker_dist[((map_differentPaths_t *)key)->position[dimension_y]]
                          [((map_differentPaths_t *)key)->position[dimension_x]] -
          world.hiker_dist[((map_differentPaths_t *)with)->position[dimension_y]]
                          [((map_differentPaths_t *)with)->position[dimension_x]]);
}

static int32_t rival_characterMainPlayer(const void *key, const void *with)
{
  return (world.rival_dist[((map_differentPaths_t *)key)->position[dimension_y]]
                          [((map_differentPaths_t *)key)->position[dimension_x]] -
          world.rival_dist[((map_differentPaths_t *)with)->position[dimension_y]]
                          [((map_differentPaths_t *)with)->position[dimension_x]]);
}
static int32_t swimmer_characterMainPlayer(const void *key, const void *with)
{
  return (world.swimmer_dist[((map_differentPaths_t *)key)->position[dimension_y]]
                          [((map_differentPaths_t *)key)->position[dimension_x]] -
          world.swimmer_dist[((map_differentPaths_t *)with)->position[dimension_y]]
                          [((map_differentPaths_t *)with)->position[dimension_x]]);
}


static int32_t trainer_characterMainPlayer(const void *key, const void *with)
{
  return (((((trainer_t *)key)->cost) - ((trainer_t *)with)->cost));
}

void map_findDifferentPaths(map_t *m)
{
  heap_t h;
  uint32_t x, y;
  static map_differentPaths_t p[21][80], *c;
  static uint32_t initialized = 0;

  if (!initialized)
  {
    initialized = 1;
    for (y = 0; y < 21; y++)
    {
      for (x = 0; x < 80; x++)
      {
        p[y][x].position[dimension_y] = y;
        p[y][x].position[dimension_x] = x;
      }
    }
  }

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      world.hiker_dist[y][x] = world.rival_dist[y][x] = world.swimmer_dist[y][x] =  INT_MAX;
    }
  }
  world.hiker_dist[world.playerCharacter.position[dimension_y]][world.playerCharacter.position[dimension_x]] =
      world.rival_dist[world.playerCharacter.position[dimension_y]][world.playerCharacter.position[dimension_x]] =
      world.swimmer_dist[world.playerCharacter.position[dimension_y]][world.playerCharacter.position[dimension_x]] = 0;

  heap_init(&h, hiker_characterMainPlayer, NULL);

  for (y = 1; y < 21 - 1; y++)
  {
    for (x = 1; x < 80 - 1; x++)
    {
      if (ter_cost(x, y, character_hiker) != INT_MAX)
      {
        p[y][x].hn_character = heap_insert(&h, &p[y][x]);
      }
      else
      {
        p[y][x].hn_character = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h)))
  {
    c->hn_character = NULL;
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x] - 1].hn_character) &&
        (world.hiker_dist[c->position[dimension_y] - 1][c->position[dimension_x] - 1] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y] - 1][c->position[dimension_x] - 1] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x]].hn_character) &&
        (world.hiker_dist[c->position[dimension_y] - 1][c->position[dimension_x]] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y] - 1][c->position[dimension_x]] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x]].hn_character);
    }
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x] + 1].hn_character) &&
        (world.hiker_dist[c->position[dimension_y] - 1][c->position[dimension_x] + 1] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y] - 1][c->position[dimension_x] + 1] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x] + 1].hn_character);
    }
    if ((p[c->position[dimension_y]][c->position[dimension_x] - 1].hn_character) &&
        (world.hiker_dist[c->position[dimension_y]][c->position[dimension_x] - 1] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y]][c->position[dimension_x] - 1] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y]][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y]][c->position[dimension_x] + 1].hn_character) &&
        (world.hiker_dist[c->position[dimension_y]][c->position[dimension_x] + 1] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y]][c->position[dimension_x] + 1] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y]][c->position[dimension_x] + 1].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x] - 1].hn_character) &&
        (world.hiker_dist[c->position[dimension_y] + 1][c->position[dimension_x] - 1] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y] + 1][c->position[dimension_x] - 1] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x]].hn_character) &&
        (world.hiker_dist[c->position[dimension_y] + 1][c->position[dimension_x]] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y] + 1][c->position[dimension_x]] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x]].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x] + 1].hn_character) &&
        (world.hiker_dist[c->position[dimension_y] + 1][c->position[dimension_x] + 1] >
         world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker)))
    {
      world.hiker_dist[c->position[dimension_y] + 1][c->position[dimension_x] + 1] =
          world.hiker_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x] + 1].hn_character);
    }
  }
  heap_delete(&h);

  heap_init(&h, rival_characterMainPlayer, NULL);

  for (y = 1; y < 21 - 1; y++)
  {
    for (x = 1; x < 80 - 1; x++)
    {
      if (ter_cost(x, y, character_rival) != INT_MAX)
      {
        p[y][x].hn_character = heap_insert(&h, &p[y][x]);
      }
      else
      {
        p[y][x].hn_character = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h)))
  {
    c->hn_character = NULL;
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x] - 1].hn_character) &&
        (world.rival_dist[c->position[dimension_y] - 1][c->position[dimension_x] - 1] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y] - 1][c->position[dimension_x] - 1] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x]].hn_character) &&
        (world.rival_dist[c->position[dimension_y] - 1][c->position[dimension_x]] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y] - 1][c->position[dimension_x]] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x]].hn_character);
    }
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x] + 1].hn_character) &&
        (world.rival_dist[c->position[dimension_y] - 1][c->position[dimension_x] + 1] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y] - 1][c->position[dimension_x] + 1] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x] + 1].hn_character);
    }
    if ((p[c->position[dimension_y]][c->position[dimension_x] - 1].hn_character) &&
        (world.rival_dist[c->position[dimension_y]][c->position[dimension_x] - 1] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y]][c->position[dimension_x] - 1] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y]][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y]][c->position[dimension_x] + 1].hn_character) &&
        (world.rival_dist[c->position[dimension_y]][c->position[dimension_x] + 1] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y]][c->position[dimension_x] + 1] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y]][c->position[dimension_x] + 1].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x] - 1].hn_character) &&
        (world.rival_dist[c->position[dimension_y] + 1][c->position[dimension_x] - 1] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y] + 1][c->position[dimension_x] - 1] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x]].hn_character) &&
        (world.rival_dist[c->position[dimension_y] + 1][c->position[dimension_x]] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y] + 1][c->position[dimension_x]] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x]].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x] + 1].hn_character) &&
        (world.rival_dist[c->position[dimension_y] + 1][c->position[dimension_x] + 1] >
         world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival)))
    {
      world.rival_dist[c->position[dimension_y] + 1][c->position[dimension_x] + 1] =
          world.rival_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x] + 1].hn_character);
    }
  }
  heap_delete(&h);


  heap_init(&h, swimmer_characterMainPlayer, NULL);

  for (y = 1; y < 21 - 1; y++)
  {
    for (x = 1; x < 80 - 1; x++)
    {
      if (ter_cost(x, y, character_swimmer) != INT_MAX)
      {
        p[y][x].hn_character = heap_insert(&h, &p[y][x]);
      }
      else
      {
        p[y][x].hn_character = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h)))
  {
    c->hn_character = NULL;
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x] - 1].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y] - 1][c->position[dimension_x] - 1] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y] - 1][c->position[dimension_x] - 1] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x]].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y] - 1][c->position[dimension_x]] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y] - 1][c->position[dimension_x]] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x]].hn_character);
    }
    if ((p[c->position[dimension_y] - 1][c->position[dimension_x] + 1].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y] - 1][c->position[dimension_x] + 1] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y] - 1][c->position[dimension_x] + 1] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] - 1][c->position[dimension_x] + 1].hn_character);
    }
    if ((p[c->position[dimension_y]][c->position[dimension_x] - 1].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x] - 1] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x] - 1] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y]][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y]][c->position[dimension_x] + 1].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x] + 1] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x] + 1] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y]][c->position[dimension_x] + 1].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x] - 1].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y] + 1][c->position[dimension_x] - 1] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y] + 1][c->position[dimension_x] - 1] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x] - 1].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x]].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y] + 1][c->position[dimension_x]] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y] + 1][c->position[dimension_x]] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x]].hn_character);
    }
    if ((p[c->position[dimension_y] + 1][c->position[dimension_x] + 1].hn_character) &&
        (world.swimmer_dist[c->position[dimension_y] + 1][c->position[dimension_x] + 1] >
         world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
             ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer)))
    {
      world.swimmer_dist[c->position[dimension_y] + 1][c->position[dimension_x] + 1] =
          world.swimmer_dist[c->position[dimension_y]][c->position[dimension_x]] +
          ter_cost(c->position[dimension_x], c->position[dimension_y], character_swimmer);
      heap_decrease_key_no_replace(&h,
                                   p[c->position[dimension_y] + 1][c->position[dimension_x] + 1].hn_character);
    }
  }
  heap_delete(&h);
}

void init_differentPlayerCharacters()
{
  int x, y;

  do
  {
    x = rand() % (80 - 2) + 1;
    y = rand() % (21 - 2) + 1;
  } while (world.cur_idx->map[y][x] != terrain_mapPath);

  world.playerCharacter.position[dimension_x] = x;
  world.playerCharacter.position[dimension_y] = y;
}

void initializeTrainers(uint16_t number_DifferentTrainers, map_t *m)
{
  int x, y;

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      world.trainers[y][x].display = false;
    }
  }

  if (number_DifferentTrainers == 1)
  {
    do
    {
      x = rand() % (80 - 2) + 1;
      y = rand() % (21 - 2) + 1;
    } while (world.hiker_dist[y][x] == INT_MAX);
    world.trainers[y][x].position[0] = x;
    world.trainers[y][x].position[1] = y;
    world.trainers[y][x].next_position[0] = x;
    world.trainers[y][x].next_position[1] = y;
    world.trainers[y][x].type = character_hiker;
    world.trainers[y][x].direction = 'z';
    world.trainers[y][x].ter = m->map[y][x];
    world.trainers[y][x].cost = 0;
    world.trainers[y][x].display = true;
  }
  else if (number_DifferentTrainers == 2)
  {
    do
    {
      x = rand() % (80 - 2) + 1;
      y = rand() % (21 - 2) + 1;
    } while (world.hiker_dist[y][x] == INT_MAX);
    world.trainers[y][x].position[0] = x;
    world.trainers[y][x].position[1] = y;
    world.trainers[y][x].next_position[0] = x;
    world.trainers[y][x].next_position[1] = y;
    world.trainers[y][x].type = character_hiker;
    world.trainers[y][x].direction = 'z';
    world.trainers[y][x].ter = m->map[y][x];
    world.trainers[y][x].cost = 0;
    world.trainers[y][x].display = true;

    do
    {
      x = rand() % (80 - 2) + 1;
      y = rand() % (21 - 2) + 1;
    } while ((world.rival_dist[y][x] == INT_MAX) && (!world.trainers[y][x].display));
    world.trainers[y][x].position[0] = x;
    world.trainers[y][x].position[1] = y;
    world.trainers[y][x].next_position[0] = x;
    world.trainers[y][x].next_position[1] = y;
    world.trainers[y][x].type = character_rival;
    world.trainers[y][x].direction = 'z';
    world.trainers[y][x].ter = m->map[y][x];
    world.trainers[y][x].cost = 0;
    world.trainers[y][x].display = true;
  }
  else if (number_DifferentTrainers > 2)
  {
    do
    {
      x = rand() % (80 - 2) + 1;
      y = rand() % (21 - 2) + 1;
    } while (world.hiker_dist[y][x] == INT_MAX);
    world.trainers[y][x].position[0] = x;
    world.trainers[y][x].position[1] = y;
    world.trainers[y][x].next_position[0] = x;
    world.trainers[y][x].next_position[1] = y;
    world.trainers[y][x].type = character_hiker;
    world.trainers[y][x].direction = 'z';
    world.trainers[y][x].ter = m->map[y][x];
    world.trainers[y][x].cost = 0;
    world.trainers[y][x].display = true;

    do
    {
      x = rand() % (80 - 2) + 1;
      y = rand() % (21 - 2) + 1;
    } while (((world.rival_dist[y][x] == INT_MAX) && (!world.trainers[y][x].display)) || m->map[y][x] == terrain_water || m->map[y][x] == terrain_mountains || m->map[y][x] == terrain_forest);
    world.trainers[y][x].position[0] = x;
    world.trainers[y][x].position[1] = y;
    world.trainers[y][x].next_position[0] = x;
    world.trainers[y][x].next_position[1] = y;
    world.trainers[y][x].type = character_rival;
    world.trainers[y][x].direction = 'z';
    world.trainers[y][x].ter = m->map[y][x];
    world.trainers[y][x].cost = 0;
    world.trainers[y][x].display = true;

    int i;
    for (i = 2; i < number_DifferentTrainers; i++)
    {
      int r = rand_range(1, countDifferentCharacters - 1);
      do
      {
        x = rand() % (80 - 2) + 1;
        y = rand() % (21 - 2) + 1;
        if (m->map[y][x] == terrain_water)
        {
          r = character_swimmer;
        }
      } while (((world.rival_dist[y][x] == INT_MAX) && m->map[y][x] == terrain_mapPath && (!world.trainers[y][x].display)) || (m->map[y][x] == terrain_water && r != character_swimmer) || m->map[y][x] == terrain_mountains || m->map[y][x] == terrain_forest); // Use rival distance so the player can reach the trainer, regardless of what gets spawned
      world.trainers[y][x].position[0] = x;
      world.trainers[y][x].position[1] = y;
      world.trainers[y][x].next_position[0] = x;
      world.trainers[y][x].next_position[1] = y;
      if (r == character_other || (r == character_swimmer && m->map[y][x] != terrain_water))
      {
        r = character_explorer;
      }
      world.trainers[y][x].type = r;
      world.trainers[y][x].direction = 'z';
      world.trainers[y][x].ter = m->map[y][x];
      world.trainers[y][x].cost = 0;
      world.trainers[y][x].display = true;
    }
  }
}
void print_rivalCharacter()
{
  int x, y;

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      if (world.rival_dist[y][x] == INT_MAX || world.rival_dist[y][x] < 0)
      {
        printf("   ");
      }
      else
      {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void set_LogicsNext(trainer_t *t, map_t *m)
{
  if (t->type == character_rival)
  {
    int min = INT_MAX;
    if ((t->position[0] == t->next_position[0]) && (t->position[1] == t->next_position[1]))
    {
      if (world.rival_dist[t->position[1] - 1][t->position[0] - 1] < min && world.trainers[t->position[1] - 1][t->position[0] - 1].display == false)
      { // top left
        min = world.rival_dist[t->position[1] - 1][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1] - 1;
      }
      if (world.rival_dist[t->position[1] - 1][t->position[0]] < min && world.trainers[t->position[1] - 1][t->position[0]].display == false)
      { // top
        min = world.rival_dist[t->position[1] - 1][t->position[0]];
        t->next_position[0] = t->position[0];
        t->next_position[1] = t->position[1] - 1;
      }
      if (world.rival_dist[t->position[1] - 1][t->position[0] + 1] < min && world.trainers[t->position[1] - 1][t->position[0]+1].display == false)
      { // top right
        min = world.rival_dist[t->position[1] - 1][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1];
      }
      if (world.rival_dist[t->position[1]][t->position[0] - 1] < min && world.trainers[t->position[1]][t->position[0]-1].display==false)
      { // left
        min = world.rival_dist[t->position[1]][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1];
      }
      if (world.rival_dist[t->position[1]][t->position[0] + 1] < min && world.trainers[t->position[1]][t->position[0]+1].display==false)
      { // right
        min = world.rival_dist[t->position[1]][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1];
      }
      if (world.rival_dist[t->position[1] + 1][t->position[0] - 1] < min && world.trainers[t->position[1]+1][t->position[0]-1].display==false)
      { // bottom left
        min = world.rival_dist[t->position[1] + 1][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1] + 1;
      }
      if (world.rival_dist[t->position[1] + 1][t->position[0]] < min && world.trainers[t->position[1]+1][t->position[0]].display==false)
      { // bottom
        min = world.rival_dist[t->position[1] + 1][t->position[0]];
        t->next_position[0] = t->position[0];
        t->next_position[1] = t->position[1] + 1;
      }
      if (world.rival_dist[t->position[1] + 1][t->position[0] + 1] < min && world.trainers[t->position[1]+1][t->position[0]+1].display==false)
      { // bottom right
        min = world.rival_dist[t->position[1] + 1][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1] + 1;
      }
    }
    t->cost += ter_cost(t->next_position[0], t->next_position[1], character_rival);
  }
  else if (t->type == character_hiker)
  {
    int min = INT_MAX;
    if ((t->position[0] == t->next_position[0]) && (t->position[1] == t->next_position[1]))
    {
      if (world.hiker_dist[t->position[1] - 1][t->position[0] - 1] < min && world.trainers[t->position[1]-1][t->position[0]-1].display==false)
      { // top left
        min = world.hiker_dist[t->position[1] - 1][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1] - 1;
      }
      if (world.hiker_dist[t->position[1] - 1][t->position[0]] < min && world.trainers[t->position[1]-1][t->position[0]].display==false)
      { // top
        min = world.hiker_dist[t->position[1] - 1][t->position[0]];
        t->next_position[0] = t->position[0];
        t->next_position[1] = t->position[1] - 1;
      }
      if (world.hiker_dist[t->position[1] - 1][t->position[0] + 1] < min && world.trainers[t->position[1]-1][t->position[0]+1].display==false)
      { // top right
        min = world.hiker_dist[t->position[1] - 1][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1];
      }
      if (world.hiker_dist[t->position[1]][t->position[0] - 1] < min && world.trainers[t->position[1]][t->position[0]-1].display==false)
      { // left
        min = world.hiker_dist[t->position[1]][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1];
      }
      if (world.hiker_dist[t->position[1]][t->position[0] + 1] < min && world.trainers[t->position[1]][t->position[0]+1].display==false)
      { // right
        min = world.hiker_dist[t->position[1]][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1];
      }
      if (world.hiker_dist[t->position[1] + 1][t->position[0] - 1] < min && world.trainers[t->position[1]+1][t->position[0]-1].display==false)
      { // bottom left
        min = world.hiker_dist[t->position[1] + 1][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1] + 1;
      }
      if (world.hiker_dist[t->position[1] + 1][t->position[0]] < min && world.trainers[t->position[1]+1][t->position[0]].display==false)
      { // bottom
        min = world.hiker_dist[t->position[1] + 1][t->position[0]];
        t->next_position[0] = t->position[0];
        t->next_position[1] = t->position[1] + 1;
      }
      if (world.hiker_dist[t->position[1] + 1][t->position[0] + 1] < min && world.trainers[t->position[1]+1][t->position[0]+1].display==false)
      { // bottom right
        min = world.hiker_dist[t->position[1] + 1][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1] + 1;
      }
    }
    t->cost += ter_cost(t->next_position[0], t->next_position[1], character_hiker);
  }

  else if (t->type == character_pacer)
  {
    if (t->direction == 'z')
    {
      int d = rand_range(0, 3);
      if (d == 0)
      {
        t->direction = 'n';
      }
      else if (d == 1)
      {
        t->direction = 's';
      }
      else if (d == 2)
      {
        t->direction = 'e';
      }
      else if (d == 3)
      {
        t->direction = 'w';
      }
    }

    if (t->direction == 'n')
    {

      if (((ter_cost(t->position[0], t->position[1] - 1, character_other)) != INT_MAX)&& world.trainers[t->position[0]][t->position[0]-1].display==false)
      {
        if (!world.trainers[t->position[1] - 1][t->position[0]].display)
        {
          t->next_position[0] = t->position[0];
          t->next_position[1] = t->position[1] - 1;
        }
      }
      else if (((ter_cost(t->position[0], t->position[1] + 1, character_other)) != INT_MAX)&& world.trainers[t->position[1]+1][t->position[0]].display==false)
      {
        if (!world.trainers[t->position[1] + 1][t->position[0]].display)
        {
          t->next_position[0] = t->position[0];
          t->next_position[1] = t->position[1] + 1;
          t->direction = 's';
        }
      }

    }
    else if (t->direction == 's')
    {
      if ((ter_cost(t->position[0], t->position[1] + 1, character_other) != INT_MAX)&& world.trainers[t->position[1]+1][t->position[0]].display==false)
      {
        if ((!world.trainers[t->position[1] + 1][t->position[0]].display))
        {
          t->next_position[0] = t->position[0];
          t->next_position[1] = t->position[1] + 1;
        }
      }
      else if (((ter_cost(t->position[0], t->position[1] - 1, character_other)) != INT_MAX)&& world.trainers[t->position[1]-1][t->position[0]].display==false)
      {
        if ((!world.trainers[t->position[1] - 1][t->position[0]].display))
        {
          t->next_position[0] = t->position[0];
          t->next_position[1] = t->position[1] - 1;
          t->direction = 'n';
        }
      }
    }
    else if (t->direction == 'e')
    {

      if (((ter_cost(t->position[0] + 1, t->position[1], character_other)) != INT_MAX)&& world.trainers[t->position[1]][t->position[0]+1].display==false)
      {
        if (!world.trainers[t->position[1]][t->position[0] + 1].display)
        {
          t->next_position[0] = t->position[0] + 1;
          t->next_position[1] = t->position[1];
        }
      }
      else if ((ter_cost(t->position[0] - 1, t->position[1], character_other) != INT_MAX)&& world.trainers[t->position[1]][t->position[0]-1].display==false)
      {
        if (!world.trainers[t->position[1]][t->position[0] - 1].display)
        {
          t->next_position[0] = t->position[0] - 1;
          t->next_position[1] = t->position[1];
          t->direction = 'w';
        }
      }
    }
    else if (t->direction == 'w')
    {
      if (((ter_cost(t->position[0] - 1, t->position[1], character_other)) != INT_MAX)&& world.trainers[t->position[1]][t->position[0]-1].display==false)
      {
        if (!world.trainers[t->position[1]][t->position[0] - 1].display)
        {
          t->next_position[0] = t->position[0] - 1;
          t->next_position[1] = t->position[1];
        }
      }
      else if (((ter_cost(t->position[0] + 1, t->position[1], character_other)) != INT_MAX)&& world.trainers[t->position[1]][t->position[0]+1].display==false)
      {
        if (!world.trainers[t->position[1]][t->position[0] + 1].display)
        {
          t->next_position[0] = t->position[0] + 1;
          t->next_position[1] = t->position[1];
          t->direction = 'e';
        }
      }
    }

    if (t->cost != INT_MAX)
    {
      t->cost += ter_cost(t->next_position[0], t->next_position[1], character_other);
    }
  }
  else if (t->type == character_wanderer)
  {
    if (t->direction == 'z')
    {
      int d = rand_range(0, 3);
      if (d == 0)
      {
        t->direction = 'n';
      }
      else if (d == 1)
      {
        t->direction = 's';
      }
      else if (d == 2)
      {
        t->direction = 'e';
      }
      else if (d == 3)
      {
        t->direction = 'w';
      }
    }
    else
    {
      if (t->direction == 'n')
      {
        if (((ter_cost(t->position[0], t->position[1] - 1, character_other)) != INT_MAX) && m->map[t->position[1] - 1][t->position[0]] == t->ter && world.trainers[t->position[1]-1][t->position[0]].display==false)
        {
          if (!world.trainers[t->position[1] - 1][t->position[0]].display)
          {
            t->next_position[0] = t->position[0];
            t->next_position[1] = t->position[1] - 1;
          }
        }
        else if (((ter_cost(t->position[0], t->position[1] + 1, character_other)) != INT_MAX) && m->map[t->position[1] + 1][t->position[0]] == t->ter && world.trainers[t->position[1]+1][t->position[0]].display==false)
        {
          if (!world.trainers[t->position[1] + 1][t->position[0]].display)
          {
            t->next_position[0] = t->position[0];
            t->next_position[1] = t->position[1] + 1;
            t->direction = 's';
          }
        }
      }
      else if (t->direction == 's')
      {
        if ((t->position[0] == t->next_position[0]) && (t->position[1] == t->next_position[1]))
        {
          if (((ter_cost(t->position[0], t->position[1] + 1, character_other)) != INT_MAX) && m->map[t->position[1] + 1][t->position[0]] == t->ter && world.trainers[t->position[1]+1][t->position[0]].display==false)
          {
            if (!world.trainers[t->position[1] + 1][t->position[0]].display)
            {
              t->next_position[0] = t->position[0];
              t->next_position[1] = t->position[1] + 1;
            }
          }
          else if (((ter_cost(t->position[0], t->position[1] - 1, character_other)) != INT_MAX) && m->map[t->position[1] - 1][t->position[0]] == t->ter && world.trainers[t->position[1]-1][t->position[0]].display==false)
          {
            if (!world.trainers[t->position[1] - 1][t->position[0]].display)
            {
              t->next_position[0] = t->position[0];
              t->next_position[1] = t->position[1] - 1;
              t->direction = 'n';
            }
          }
        }
      }
      else if (t->direction == 'e')
      {
        if (((ter_cost(t->position[0] + 1, t->position[1], character_other)) != INT_MAX) && m->map[t->position[1]][t->position[0] + 1] == t->ter && world.trainers[t->position[1]][t->position[0]+1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] + 1].display)
          {
            t->next_position[0] = t->position[0] + 1;
            t->next_position[1] = t->position[1];
          }
        }
        else if (((ter_cost(t->position[0] - 1, t->position[1], character_other)) != INT_MAX) && m->map[t->position[1]][t->position[0] - 1] == t->ter && world.trainers[t->position[1]][t->position[0]-1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] - 1].display)
          {
            t->next_position[0] = t->position[0] - 1;
            t->next_position[1] = t->position[1];
            t->direction = 'w';
          }
        }
      }
      else if (t->direction == 'w')
      {
        if (((ter_cost(t->position[0] - 1, t->position[1], character_other)) != INT_MAX) && m->map[t->position[1]][t->position[0] - 1] == t->ter && world.trainers[t->position[1]][t->position[0]-1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] - 1].display)
          {
            t->next_position[0] = t->position[0] - 1;
            t->next_position[1] = t->position[1];
          }
        }
        else if (((ter_cost(t->position[0] + 1, t->position[1], character_other)) != INT_MAX) && m->map[t->position[1]][t->position[0] + 1] == t->ter && world.trainers[t->position[1]][t->position[0]+1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] + 1].display)
          {
            t->next_position[0] = t->position[0] + 1;
            t->next_position[1] = t->position[1];
            t->direction = 'e';
          }
        }
      }
    }
    if (t->cost != INT_MAX)
    {
      t->cost += ter_cost(t->next_position[0], t->next_position[1], character_other);
    }
  }

  else if (t->type == character_swimmer)
  { 
      int status = 0;
      int xcoord, ycoord;
  for (ycoord = 0; ycoord < 21; ycoord++){
    for (xcoord = 0; xcoord < 80; xcoord++){
      if (world.playerCharacter.position[dimension_x] == xcoord && world.playerCharacter.position[dimension_y] == ycoord
      && (world.cur_idx->map[ycoord][xcoord] == terrain_gate || world.cur_idx->map[ycoord][xcoord] == terrain_bridge || world.cur_idx->map[ycoord][xcoord] == terrain_mapPath))
      { //Set status to 1 if the pc is on a path  
        status = 1;
      }
    }
  }
  if(status == 1){ //Take fastest path to pc
    int min = INT_MAX;
    if ((t->position[0] == t->next_position[0]) && (t->position[1] == t->next_position[1]))
    {
      if (world.swimmer_dist[t->position[1] - 1][t->position[0] - 1] < min&& world.trainers[t->position[1]-1][t->position[0]-1].display==false)
      { // top left
        min = world.swimmer_dist[t->position[1] - 1][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1] - 1;
      }
      if (world.swimmer_dist[t->position[1] - 1][t->position[0]] < min && world.trainers[t->position[1]-1][t->position[0]].display==false)
      { // top
        min = world.swimmer_dist[t->position[1] - 1][t->position[0]];
        t->next_position[0] = t->position[0];
        t->next_position[1] = t->position[1] - 1;
      }
      if (world.swimmer_dist[t->position[1] - 1][t->position[0] + 1] < min && world.trainers[t->position[1]-1][t->position[0]+1].display==false)
      { // top right
        min = world.swimmer_dist[t->position[1] - 1][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1];
      }
      if (world.swimmer_dist[t->position[1]][t->position[0] - 1] < min && world.trainers[t->position[1]][t->position[0]-1].display==false)
      { // left
        min = world.swimmer_dist[t->position[1]][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1];
      }
      if (world.swimmer_dist[t->position[1]][t->position[0] + 1] < min && world.trainers[t->position[1]][t->position[0]+1].display==false)
      { // right
        min = world.swimmer_dist[t->position[1]][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1];
      }
      if (world.swimmer_dist[t->position[1] + 1][t->position[0] - 1] < min && world.trainers[t->position[1]+1][t->position[0]-1].display==false)
      { // bottom left
        min = world.swimmer_dist[t->position[1] + 1][t->position[0] - 1];
        t->next_position[0] = t->position[0] - 1;
        t->next_position[1] = t->position[1] + 1;
      }
      if (world.swimmer_dist[t->position[1] + 1][t->position[0]] < min && world.trainers[t->position[1]+1][t->position[0]].display==false)
      { // bottom
        min = world.swimmer_dist[t->position[1] + 1][t->position[0]];
        t->next_position[0] = t->position[0];
        t->next_position[1] = t->position[1] + 1;
      }
      if (world.swimmer_dist[t->position[1] + 1][t->position[0] + 1] < min && world.trainers[t->position[1]+1][t->position[0]+1].display==false)
      { // bottom right
        min = world.swimmer_dist[t->position[1] + 1][t->position[0] + 1];
        t->next_position[0] = t->position[0] + 1;
        t->next_position[1] = t->position[1] + 1;
      }
    }
    t->cost += ter_cost(t->next_position[0], t->next_position[1], character_swimmer);
  }
  else{ //move like a wanderer 
    if (t->direction == 'z')
    {
      int d = rand_range(0, 3);
      if (d == 0)
      {
        t->direction = 'n';
      }
      else if (d == 1)
      {
        t->direction = 's';
      }
      else if (d == 2)
      {
        t->direction = 'e';
      }
      else if (d == 3)
      {
        t->direction = 'w';
      }
    }
    else
    {
      if (t->direction == 'n')
      {
        if (((ter_cost(t->position[0], t->position[1] - 1, character_swimmer)) != INT_MAX) && m->map[t->position[1] - 1][t->position[0]] == t->ter && world.trainers[t->position[1]-1][t->position[0]].display==false)
        {
          if (!world.trainers[t->position[1] - 1][t->position[0]].display)
          {
            t->next_position[0] = t->position[0];
            t->next_position[1] = t->position[1] - 1;
          }
        }
        else if (((ter_cost(t->position[0], t->position[1] + 1, character_swimmer)) != INT_MAX) && m->map[t->position[1] + 1][t->position[0]] == t->ter && world.trainers[t->position[1]+1][t->position[0]].display==false)
        {
          if (!world.trainers[t->position[1] + 1][t->position[0]].display)
          {
            t->next_position[0] = t->position[0];
            t->next_position[1] = t->position[1] + 1;
            t->direction = 's';
          }
        }
      }
      else if (t->direction == 's')
      {
        if ((t->position[0] == t->next_position[0]) && (t->position[1] == t->next_position[1]))
        {
          if (((ter_cost(t->position[0], t->position[1] + 1, character_swimmer)) != INT_MAX) && m->map[t->position[1] + 1][t->position[0]] == t->ter && world.trainers[t->position[1]+1][t->position[0]].display==false)
          {
            if (!world.trainers[t->position[1] + 1][t->position[0]].display)
            {
              t->next_position[0] = t->position[0];
              t->next_position[1] = t->position[1] + 1;
            }
          }
          else if (((ter_cost(t->position[0], t->position[1] - 1, character_swimmer)) != INT_MAX) && m->map[t->position[1] - 1][t->position[0]] == t->ter && world.trainers[t->position[1]-1][t->position[0]].display==false)
          {
            if (!world.trainers[t->position[1] - 1][t->position[0]].display)
            {
              t->next_position[0] = t->position[0];
              t->next_position[1] = t->position[1] - 1;
              t->direction = 'n';
            }
          }
        }
      }
      else if (t->direction == 'e')
      {
        if (((ter_cost(t->position[0] + 1, t->position[1], character_swimmer)) != INT_MAX) && m->map[t->position[1]][t->position[0] + 1] == t->ter && world.trainers[t->position[1]][t->position[0]+1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] + 1].display)
          {
            t->next_position[0] = t->position[0] + 1;
            t->next_position[1] = t->position[1];
          }
        }
        else if (((ter_cost(t->position[0] - 1, t->position[1], character_swimmer)) != INT_MAX) && m->map[t->position[1]][t->position[0] - 1] == t->ter && world.trainers[t->position[1]][t->position[0]-1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] - 1].display)
          {
            t->next_position[0] = t->position[0] - 1;
            t->next_position[1] = t->position[1];
            t->direction = 'w';
          }
        }
      }
      else if (t->direction == 'w')
      {
        if (((ter_cost(t->position[0] - 1, t->position[1], character_swimmer)) != INT_MAX) && m->map[t->position[1]][t->position[0] - 1] == t->ter && world.trainers[t->position[1]][t->position[0]-1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] - 1].display)
          {
            t->next_position[0] = t->position[0] - 1;
            t->next_position[1] = t->position[1];
          }
        }
        else if (((ter_cost(t->position[0] + 1, t->position[1], character_swimmer)) != INT_MAX) && m->map[t->position[1]][t->position[0] + 1] == t->ter && world.trainers[t->position[1]][t->position[0]+1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] + 1].display)
          {
            t->next_position[0] = t->position[0] + 1;
            t->next_position[1] = t->position[1];
            t->direction = 'e';
          }
        }
      }
    }
    if (t->cost != INT_MAX)
    {
      t->cost += ter_cost(t->next_position[0], t->next_position[1], character_swimmer);
    }
  }
  }

  else if (t->type == character_stationary)
  { // Stationary character doesn't move
  }
  else if (t->type == character_explorer)
  {
    int new_direction = rand_range(0, 3);
    if (t->direction == 'z')
    {
      int d = rand_range(0, 3);
      if (d == 0)
      {
        t->direction = 'n';
      }
      else if (d == 1)
      {
        t->direction = 's';
      }
      else if (d == 2)
      {
        t->direction = 'e';
      }
      else if (d == 3)
      {
        t->direction = 'w';
      }
    }
    else
    {
      if (t->direction == 'n')
      {
        if ((ter_cost(t->position[0], t->position[1] - 1, character_other)) != INT_MAX && world.trainers[t->position[1]-1][t->position[0]].display==false)
        {
          if (!world.trainers[t->position[1] - 1][t->position[0]].display)
          {
            t->next_position[0] = t->position[0];
            t->next_position[1] = t->position[1] - 1;
          }
        }
        else if (new_direction == 0 || new_direction == 1)
        {
          t->direction = 's';
        }
        else if (new_direction == 2)
        {
          t->direction = 'e';
        }
        else
        {
          t->direction = 'w';
        }
      }
      else if (t->direction == 's')
      {
        if ((ter_cost(t->position[0], t->position[1] + 1, character_other)) != INT_MAX && world.trainers[t->position[1]+1][t->position[0]].display==false)
        {
          if (!world.trainers[t->position[1] + 1][t->position[0]].display)
          {
            t->next_position[0] = t->position[0];
            t->next_position[1] = t->position[1] + 1;
          }
        }
        else if (new_direction == 0 || new_direction == 1)
        {
          t->direction = 'n';
        }
        else if (new_direction == 2)
        {
          t->direction = 'e';
        }
        else
        {
          t->direction = 'w';
        }
      }
      else if (t->direction == 'e')
      {
        if ((ter_cost(t->position[0] + 1, t->position[1], character_other)) != INT_MAX && world.trainers[t->position[1]][t->position[0]+1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] + 1].display)
          {
            t->next_position[0] = t->position[0] + 1;
            t->next_position[1] = t->position[1];
          }
        }
        else if (new_direction == 0)
        {
          t->direction = 'n';
        }
        else if (new_direction == 1)
        {
          t->direction = 's';
        }
        else
        {
          t->direction = 'w';
        }
      }
      else if (t->direction == 'w')
      {
        if ((ter_cost(t->position[0] - 1, t->position[1], character_other)) != INT_MAX && world.trainers[t->position[1]][t->position[0]-1].display==false)
        {
          if (!world.trainers[t->position[1]][t->position[0] - 1].display)
          {
            t->next_position[0] = t->position[0] - 1;
            t->next_position[1] = t->position[1];
          }
        }
        else if (new_direction == 0)
        {
          t->direction = 'n';
        }
        else if (new_direction == 1)
        {
          t->direction = 's';
        }
        else
        {
          t->direction = 'e';
        }
      }
      if (t->cost != INT_MAX)
      {
        t->cost += ter_cost(t->next_position[0], t->next_position[1], character_other);
      }
    }
  }
}

void printHikerDistance()
{
  int x, y;

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      if (world.hiker_dist[y][x] == INT_MAX)
      {
        printf("   ");
      }
      else
      {
        printf(" %02d", world.hiker_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void showTrainers()
{
  int x, y;
  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      if (world.trainers[y][x].display == false)
      {
        printf(".");
      }
      else
      {
        if (world.trainers[y][x].type == character_rival)
        {
          printf("r");
        }
        else if (world.trainers[y][x].type == character_hiker)
        {
          printf("h");
        }
        else if (world.trainers[y][x].type == character_other)
        {
          printf("o");
        }
        else if (world.trainers[y][x].type == character_pacer)
        {
          printf("p");
        }
        else if (world.trainers[y][x].type == character_swimmer) // TODO
        {
          printf("m");
        }
        else if (world.trainers[y][x].type == character_wanderer)
        {
          printf("w");
        }
        else if (world.trainers[y][x].type == character_stationary)
        {
          printf("s");
        }
        else if (world.trainers[y][x].type == character_explorer)
        {
          printf("n");
        }
      }
    }
    printf("\n");
  }
}