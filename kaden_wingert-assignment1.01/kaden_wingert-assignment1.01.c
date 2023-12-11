#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

//Global Variable which contains the contents of the map
char map[21][80];
/*
Function which prints all values stored in the 2d Array
*/
void printMap(){
    for(int y = 0; y<21; ++y){
        for(int x = 0; x<80; ++x){
            if(x == 79){
                printf("%c\n", map[y][x]);
            }
            else
                printf("%c", map[y][x]);
        }
    }
}

/*
This function is used to make the path the the character will walk on in the East/West direction
*/
void createEWPath(){

    // int randX = rand() % 76+2; //Generates random number between 2 and 77 inclusive
     int randY = rand() % 13+4; //Generates random number between 6 and 18 inclusive
     int xMidPoint = rand() % 74+3; //Generates random number between 3 and 76 inclusive...Tells you how far right to go before you go up
     int upCount = rand() % (randY-3)+2; //Creates a number smaller than the length of the space aove it

    //This nested loop creates the initial E/W path
     for(int y = 0; y < 21;++y){
        for(int x = 0; x <= xMidPoint; ++x){
            if(y == randY){
                map[y][x] = '#';
            }

        }
     }
     for(int y = upCount; y <= randY;++y){ //Creates the N/S path direction
        for(int x = 0; x < 80; ++x){
            if((x == xMidPoint) && (y <= randY))
            map[y][x] = '#';
        }
}
         for(int y = 0; y < 21;++y){
        for(int x = xMidPoint; x < 80; ++x){
            if(y == upCount)
            map[y][x] = '#';
        }
}
}
/*
This function is used to make the path the the character will walk on in the North/South direction
*/
void createNSPath(){
     int randX = rand() % 74+4; //Generates random number between 4 and 77 inclusive...Tells you how far left to go
    // int randY = rand() % 16+4; //Generates random number between 6 and 19 inclusive...Tells you how far down to go at the end
     int yMidPoint = rand() % 15+3; //Generates random number between 3 and 17 inclusive...Tells you how far down to go before you go left
     int leftCount = rand() % (randX-3)+2; //Creates a number smaller than the length of the space to the left of it

    //This nested loop creates the initial N/S path
     for(int y = 0; y <= yMidPoint;++y){
        for(int x = 0; x <= 80; ++x){
            if(x == randX){
                map[y][x] = '#';
            }

        }
     }

     for(int y = 0; y < 21 ;++y){ //Creates the E/W path direction
        for(int x = leftCount; x <= 80; ++x){
            if((y == yMidPoint) && (x <= randX))
            map[y][x] = '#';
        }
}
         for(int y = yMidPoint; y < 21;++y){
        for(int x = 0; x < 80; ++x){
            if(x == leftCount)
            map[y][x] = '#';
        }
}
}
/*
This function uses a 2d array to store the walls of the gameboard in the global variable map[][]
*/
void makeWalls(){

    for(int y = 0; y<21; ++y){ //This nested loop creates the walls to make a 21x80 map (21 rows, 80 cols)
        for(int x = 0; x<80; ++x){
            if(y == 0 || x == 0 || y == 20 ){
                map[y][x] = '%'; 
            }
            
            else if(x == 79){
                map[y][x] = '%';
            }
            
            else{
            map[y][x] = ' ';
            }
            
        }
    }
}
/*
Helper method used to determine if a single point, is valid to place a pokeCenter or pokeMart on
*/
bool validBuilding(int y, int x){
if(map[y][x] != '#' && map[y][x] != 'C' && map[y][x] != 'M' &&map[y][x] != '%'){ //If the random spot isn't already on the path and isn't a pokeCenter or pokeMart
   if(map[y][x+1] == '#' || map[y][x-1] == '#'|| map[y+1][x] == '#' || map[y-1][x] == '#')
    return true;
}
return false;
}

/*
Function that prints a single 'C' character on the board to represent a PokeCenter
*/
void pokeCenter(){
    bool placed = false;
    while(placed == false){
        int xVal = rand()%80;
        int yVal = rand()%21;
    if(validBuilding(yVal,xVal) == true){
        map[yVal][xVal] = 'C';
        placed = true;
    }

    }

}
/*
Function that prints a single 'M' character on the board to represent a Pokemart
*/
void pokeMart(){
    bool placed = false;
    while(placed == false){
        int xVal = rand()%80;
        int yVal = rand()%21;
    if(validBuilding(yVal,xVal) == true){
        map[yVal][xVal] = 'M';
        placed = true;
    }

    }

}
 /*
 This is a helper function which prints trees and boulders in the remaining spaces of the board
 */      
void remainingSpace(){
    for(int y = 0; y< 80;++y){
        for(int x = 0; x<21;++x){
            int randNum =rand();
            if(map[x][y] == ' ' && randNum %2 == 0)
                map[x][y] = ':';
            else if(map[x][y] == ' ' && randNum %2 != 0)
                map[x][y] = '.';
            
        }
    }
    int randNum = rand()%51+10;  // Value: 10-60
    for(int i = 0; i<randNum; ++i){
        int randX = rand()%79+1; // Value: 1-79
        int randY = rand()%20+1; // Value: 1-20
        int randNum2 = rand() %2;
        if(map[randY][randX] != '%' && map[randY][randX] !='~'){
            if (randNum2 ==0)
            map[randY][randX] = '"';
            else
            map[randY][randX] = '%';
        }
    }
}

/*
This function creates 3 patches of tall grass and 2 patches of short grass
*/
void terrain(){
    //First area for water
    int waterX1 = rand()%77+1; // Value: 1-77
    int waterY1 = rand()%18+1; //Value: 1-18
    int xExpand7 = rand()%21+20; //Value: 20-40
    int yExpand7 = rand()%11+10; //Value 10-20
    for(int y = waterY1; y < (waterY1+yExpand7);++y){
        for(int x = waterX1; x < (waterX1+ xExpand7);++x){
            if(map[y][x] != '%')
                map[y][x] = '~';
            
        }
    }
    //Second area for water
    int waterX2 = rand()%77+1; // Value: 1-77
    int waterY2 = rand()%18+1; //Value: 1-18
    int xExpand8 = rand()%51+5; //Value: 5-55
    int yExpand8 = rand()%11+5; //Value 5-15
    for(int y = waterY2; y < (waterY2+yExpand8);++y){
        for(int x = waterX2; x < (waterX2+ xExpand8);++x){
            if(map[y][x] != '%')
                map[y][x] = '~';
            
        }
    }
    //1st section for boulders
    int boulderX1 = rand()%77+1; // Value: 1-77
    int boulderY1 = rand()%18+1; //Value: 1-18
    int xExpand9 = rand()%51+5; //Value: 5-55
    int yExpand9 = rand()%11+5; //Value 5-15
    for(int y = boulderY1; y < (boulderY1+yExpand9);++y){
        for(int x = boulderX1; x < (boulderX1+ xExpand9);++x){
            if(map[y][x] != '%')
                map[y][x] = '~';
            
        }
    }
    //1st section of the tall grass
    int longGrassX1 = rand()%77+1; // Value: 1-77
    int longGrassY1 = rand()%18+1; //Value: 1-18
    int xExpand1 = rand()%21+50; //Value: 50-70
    int yExpand1 = rand()%11+8; //Value 8-18
    for(int y = longGrassY1; y < (longGrassY1+ yExpand1);++y){
        for(int x = longGrassX1; x < (longGrassX1 + xExpand1);++x){
            if(map[y][x] != '%')
                map[y][x] = ':';
            
        }
    }

    //2nd section of the tall grass
    int longGrassX2 = rand()%77+1; // Value: 1-77
    int longGrassY2 = rand()%18+1; //Value: 1-18
    int xExpand2 = rand()%21+8; //Value: 8-28
    int yExpand2 = rand()%8+8; //Value 8-15
    while(longGrassX2 != longGrassX1 && longGrassY2 != longGrassY1){
         longGrassX2 = rand()%77+1; // Value: 1-77
         longGrassY2 = rand()%18+1; //Value: 1-18
    }
    for(int y = longGrassY2; y < (longGrassY2+ yExpand2);++y){
        for(int x = longGrassX2; x < (longGrassX2+ xExpand2);++x){
            if(map[y][x] != '%')
                map[y][x] = ':';
            
        }
    }
    //3rd section of the tall grass
    int longGrassX3 = rand()%77+1; // Value: 1-77
    int longGrassY3 = rand()%18+1; //Value: 1-18
    int xExpand3 = rand()%61+1; //Value: 1-60
    int yExpand3 = rand()%18+1; //Value 1-18
    while(longGrassX3 != longGrassX1 && longGrassY3 != longGrassY1){
         longGrassX3 = rand()%77+1; // Value: 1-77
         longGrassY3 = rand()%18+1; //Value: 1-18
    }
    for(int y = longGrassY3; y < (longGrassY3+ yExpand3);++y){
        for(int x = longGrassX3; x < (longGrassX3+ xExpand3);++x){
            if(map[y][x] != '%')
                map[y][x] = ':';
            
        }
    }
     //1st section of the short grass
    int shortGrassX1 = rand()%77+1; // Value: 1-77
    int shortGrassY1 = rand()%18+1; //Value: 1-18
    int xExpand4 = rand()%21+40; //Value: 40-60
    int yExpand4 = rand()%11+10; //Value 10-20
    for(int y = shortGrassY1; y < (shortGrassY1+yExpand4);++y){
        for(int x = shortGrassX1; x < (shortGrassX1+ xExpand4);++x){
            if(map[y][x] != '%')
                map[y][x] = '.';
            
        }
    }
    //2nd section of the short grass
    int shortGrassX2 = rand()%77+1; // Value: 1-77
    int shortGrassY2 = rand()%18+1; //Value: 1-18
    int xExpand5 = rand()%31+10; //Value: 10-40
    int yExpand5 = rand()%8+8; //Value 8-15
    while(shortGrassX2 != shortGrassX1 && shortGrassY2 != shortGrassY1){
         shortGrassX2 = rand()%77+1; // Value: 1-77
         shortGrassY2 = rand()%18+1; //Value: 1-18
    }
    for(int y = shortGrassY2; y < (shortGrassY2+yExpand5);++y){
        for(int x = shortGrassX2; x < (shortGrassX2+ xExpand5);++x){
            if(map[y][x] != '%')
                map[y][x] = '.';
            
        }
    }
     //3rd section of the short grass
    int shortGrassX3 = rand()%77+1; // Value: 1-77
    int shortGrassY3 = rand()%18+1; //Value: 1-18
    int xExpand6 = rand()%31+40; //Value: 40-70
    int yExpand6 = rand()%18+1; //Value 1-18
    while(shortGrassX3 != shortGrassX1 && shortGrassY3 != shortGrassY1){
         shortGrassX3 = rand()%77+1; // Value: 1-77
         shortGrassY3 = rand()%18+1; //Value: 1-18
    }
    for(int y = shortGrassY3; y < (shortGrassY3+yExpand6);++y){
        for(int x = shortGrassX3; x < (shortGrassX3+ xExpand6);++x){
            if(map[y][x] != '%')
                map[y][x] = '.';
            
        }
    }
    remainingSpace();
 
}

/*This is the main function that is used to display the board
*/
void printBoard(){
    makeWalls(); //Must come first
    terrain();
    createEWPath(); //Paths must come after the required terrain
    createNSPath();
    pokeCenter(); //Buildings must come after paths
    pokeMart();
    printMap();
}

int main(int argc, char*argv[]){
       srand(time(NULL));
    printBoard();
    return 0;
}