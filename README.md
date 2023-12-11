
# Kaden's Pokemon Game

Welcome to Ryan Huellen's Pokemon Game! This is a rogue-like text-based game written in C, heavily inspired by the Pokemon franchise. Players will embark on an adventure, capturing and battling creatures in a procedurally generated world.


## Features

- Randomly generated world with various region types, Pokemon Centers, and Pokemarts.
- Upon entering a room, your player character '@' is placed on the path
- 6 types of trainers exist. We have Hikers, Rivals, Pacers, Wanderers, Sentries, and Explorers.
- Player movement in all eight directions 
- Pokemart interface
- Pokemon Center interface
- Trainer interface
- Walking and flying between maps
- Pokemon database
- Ability to choose a starter Pokemon from 3 options
- Ability to capture wild Pokemon
- You can fight trainers and wild Pokemon through battle interfaces
- Potions (heal Pokemon)
- Revives (revive Pokemon or heal to full health)
- Pokeballs (capture wild Pokemon)
- Inventory system

## Game Data

Data is loaded in from one of three locations, relative to the executable, including:

- `/share/cs327/pokedex/pokedex/data/csv/`
- `~/.cs327/`
- `../data/`

The following data types are loaded:

- Experience (experience.csv)
- Moves (moves.csv)
- Pokemon (pokemon.csv)
- Pokemon Moves (pokemon_moves.csv)
- Pokemon Species (pokemon_species.csv)
- Pokemon Stats (pokemon_stats.csv)
- Pokemon Types (pokemon_types.csv)
- Stats (stats.csv)
- Type Names (type_names.csv)

## Trainers

In order to pick the number of trainers that spawn on the map, you can run the binary with the `--numtrainers` flag. Here's a sample of choosing 7 trainers:

`./main --numtrainers 7`


### Hikers and Rivals

Hikers and Rivals follow an efficient path to the player based on their respective cost maps. Note that if there is no valid path to the player as that player is blocked by other trainers or immovable terrain, then hikers and rivals will not move. Moreover, if a Hiker or Rival is defeated, they will instead choose to move in a random direction, rather than pathing to the player.

### Pacers

Pacers choose a single direction and move in that direction until they hit an immovable object. Then, they turn around and repeat.

### Wanderers

Wanderers move in a random directon and continue that direction until they reach the end of their current terrain type. Then, they pick another direction to stay within the terrain limits.

### Explorers

Explorers move like wanderers, except they can leave the current terrain type.

### Sentries

Sentries don't move at all! Be wary!

## Buildings

The game features two buildings, a Pokemon Center (C) and a Pokemart (M). Upon pathing to a respective building, you can open the building's interface with the `>` key. In order to close the interface, press the `<` key.

### Pokemon Center

Upon entering a Pokemon Center, all of your pokemon are healed to full health.

### Pokemart

Upon entering a Pokemart, all of your supplies are replenished. This includes potions, revives, and pokeballs.

### Battle Interface

The battle interface is fairly simple. You can do one of the following:

1. Fight
2. Bag
3. Run
4. Pokemon

#### Fight

When you select fight, you'll be presented with a list of moves. You can select a move by using number keys. Upon selecting a move, you will perform the move! Note, however, if your move's priority is less than that of the opponent and they also choose to fight, they will attack first.

#### Bag

When you select bag, you'll be presented with the ability to use a potion, revive, or, if you're in a wild battle, a pokeball. You can select an item by using number keys. Upon selecting an item, you will use the item!

#### Run

When you select run, you'll attempt to run from the battle. You can only run from a wild pokemon battle. You also have a 50% chance of failing to run. Note, this will use your turn!

#### Pokemon

When you select pokemon, you'll be presented with a list of your pokemon. You can select a pokemon by using number keys. Upon selecting a pokemon, you will switch to that pokemon! Note, however, switching pokemon uses a turn.
