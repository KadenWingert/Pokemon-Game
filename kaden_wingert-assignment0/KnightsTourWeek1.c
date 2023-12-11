/*
 * @author Kaden Wingert
 */

#include <stdio.h>
#include <stdbool.h>
#define N 5

//Together, xMove and yMove allow you to test all 8 combinations that the knight can possibly move.
int xMove[8] = {  2, 1, -1, -2, -2, -1,  1,  2 };
int yMove[8] = {  1, 2,  2,  1, -1, -2, -2, -1 };

/*
 * Prints the path that the knight traveled
 */
void printSolution(int solArr[N][N]) {
    for (int i = 0; i < N*N; i++) {
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                if (solArr[x][y] == i) {
                    printf("%d", x*N + y + 1);
                    if (i != N*N-1) {
                        printf(",");
                    }
                    break;
                }
            }
        }
    }
    printf("\n");
}

/*
 * Determines if the knight is allowed to move in this square. Returns false if the move's index is >4 or <0.
 * If it IS within the constraints of the board, returns true
 */
bool validMove(int x, int y, int solArr[N][N]) {
    bool valid = (x >= 0 && x < N && y >= 0 && y < N && solArr[x][y] == -1);
    return valid;
}
/*
 * This function uses recursion to test every path that the knight can travel
 */
bool tryPath(int x, int y, int moveCount, int solArr[N][N], int xMove[N], int yMove[N]) {
    int k, next_x, next_y;
    //If the knight has traveled all 25 squares on the board, print the path it took
    if (moveCount == N*N) {
        printSolution(solArr);
        return false;
    }
    //Goes through all combinations that the knight is able to move
    for (k = 0; k < 8; k++) {
        //Updates the x and y values with the knights move
        next_x = x + xMove[k];
        next_y = y + yMove[k];
        if (validMove(next_x, next_y, solArr)) {
            //Adds the number of moves that the knight has traveled to the array which stores the solution
            solArr[next_x][next_y] = moveCount;

            //Recursively calls itself while incrementing the move count
            if (tryPath(next_x, next_y, moveCount+1, solArr, xMove, yMove) == true)
                return true;

            //Backtracks if there are no valid moves from the current move
            else
                solArr[next_x][next_y] = -1;
        }
    }
    // If there is no solution with this path
    return false;
}

/*
 *The main function that is in charge of finding the moves that complete the Knight's tour
 */
void solver() {
    int solutionArr[N][N];

    //Iterates through every possible starting position on the board
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {

            //Sets all squares to "unvisited"
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    solutionArr[i][j] = -1;
                }
            }
            //Since you start on a square, set the square to "visited" and try all paths from that spot
            solutionArr[x][y] = 0;
            tryPath(x, y, 1, solutionArr, xMove, yMove);
        }
    }
}
int main() {
    solver();
    return 0;
}