all: main

main: tile.o trainer.o player.o room.o map.o game.o queue.o priorityqueue.o render.o menu.o battle.o debug.o gamedata.o move.o stat.o experience.o typename.o pokemonmove.o pokemonspecies.o pokemonstat.o pokemontype.o pokemon.o csv.o cohesivepokemon.o main.o
	g++ tile.o trainer.o player.o room.o map.o game.o queue.o priorityqueue.o render.o menu.o battle.o debug.o gamedata.o move.o stat.o experience.o typename.o pokemonmove.o pokemonspecies.o pokemonstat.o pokemontype.o pokemon.o csv.o cohesivepokemon.o main.o -o main -Wall -Werror -lm -lncurses

tile.o: ./lib/tile/tile.cpp ./lib/tile/tile.h
	g++ -c ./lib/tile/tile.cpp -o tile.o -Wall -Werror

trainer.o: ./lib/trainer/trainer.cpp ./lib/trainer/trainer.h
	g++ -c ./lib/trainer/trainer.cpp -o trainer.o -Wall -Werror

player.o: ./lib/player/player.cpp ./lib/player/player.h
	g++ -c ./lib/player/player.cpp -o player.o -Wall -Werror

room.o: ./lib/room/room.cpp ./lib/room/room.h
	g++ -c ./lib/room/room.cpp -o room.o -Wall -Werror

map.o: ./lib/map/map.cpp ./lib/map/map.h
	g++ -c ./lib/map/map.cpp -o map.o -Wall -Werror

game.o: ./lib/game/game.cpp ./lib/game/game.h
	g++ -c ./lib/game/game.cpp -o game.o -Wall -Werror

queue.o: ./util/queue/queue.c ./util/queue/queue.h
	g++ -x c -c ./util/queue/queue.c -o queue.o -Wall -Werror

priorityqueue.o: ./util/priorityqueue/priorityqueue.c ./util/priorityqueue/priorityqueue.h
	g++ -x c -c ./util/priorityqueue/priorityqueue.c -o priorityqueue.o -Wall -Werror

render.o: ./lib/render/render.cpp ./lib/render/render.h
	g++ -c ./lib/render/render.cpp -o render.o -Wall -Werror

menu.o: ./lib/menu/menu.cpp ./lib/menu/menu.h
	g++ -c ./lib/menu/menu.cpp -o menu.o -Wall -Werror

battle.o: ./lib/battle/battle.cpp ./lib/battle/battle.h
	g++ -c ./lib/battle/battle.cpp -o battle.o -Wall -Werror

debug.o: ./util/debug/debug.cpp ./util/debug/debug.h
	g++ -c ./util/debug/debug.cpp -o debug.o -Wall -Werror

gamedata.o: ./lib/gamedata/gamedata.cpp ./lib/gamedata/gamedata.h
	g++ -c ./lib/gamedata/gamedata.cpp -o gamedata.o -Wall -Werror

csv.o: ./util/csv/csv.cpp ./util/csv/csv.h
	g++ -c ./util/csv/csv.cpp -o csv.o -Wall -Werror

move.o: ./lib/gamedata/move/move.cpp ./lib/gamedata/move/move.h
	g++ -c ./lib/gamedata/move/move.cpp -o move.o -Wall -Werror

stat.o: ./lib/gamedata/stat/stat.cpp ./lib/gamedata/stat/stat.h
	g++ -c ./lib/gamedata/stat/stat.cpp -o stat.o -Wall -Werror

experience.o: ./lib/gamedata/experience/experience.cpp ./lib/gamedata/experience/experience.h
	g++ -c ./lib/gamedata/experience/experience.cpp -o experience.o -Wall -Werror

typename.o: ./lib/gamedata/typename/typename.cpp ./lib/gamedata/typename/typename.h
	g++ -c ./lib/gamedata/typename/typename.cpp -o typename.o -Wall -Werror

pokemonmove.o: ./lib/gamedata/pokemonmove/pokemonmove.cpp ./lib/gamedata/pokemonmove/pokemonmove.h
	g++ -c ./lib/gamedata/pokemonmove/pokemonmove.cpp -o pokemonmove.o -Wall -Werror

pokemonspecies.o: ./lib/gamedata/pokemonspecies/pokemonspecies.cpp ./lib/gamedata/pokemonspecies/pokemonspecies.h
	g++ -c ./lib/gamedata/pokemonspecies/pokemonspecies.cpp -o pokemonspecies.o -Wall -Werror

pokemonstat.o: ./lib/gamedata/pokemonstat/pokemonstat.cpp ./lib/gamedata/pokemonstat/pokemonstat.h
	g++ -c ./lib/gamedata/pokemonstat/pokemonstat.cpp -o pokemonstat.o -Wall -Werror

pokemontype.o: ./lib/gamedata/pokemontype/pokemontype.cpp ./lib/gamedata/pokemontype/pokemontype.h
	g++ -c ./lib/gamedata/pokemontype/pokemontype.cpp -o pokemontype.o -Wall -Werror

pokemon.o: ./lib/gamedata/pokemon/pokemon.cpp ./lib/gamedata/pokemon/pokemon.h
	g++ -c ./lib/gamedata/pokemon/pokemon.cpp -o pokemon.o -Wall -Werror

cohesivepokemon.o: ./lib/cohesivepokemon/cohesivepokemon.cpp ./lib/cohesivepokemon/cohesivepokemon.h
	g++ -c ./lib/cohesivepokemon/cohesivepokemon.cpp -o cohesivepokemon.o -Wall -Werror

main.o: main.cpp
	g++ -c main.cpp -o main.o -Wall -Werror

clean:
	rm -rf main *.o *.dSYM *.txt