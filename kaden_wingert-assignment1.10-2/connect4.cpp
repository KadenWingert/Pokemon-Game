#include <iostream>
#include <cstdlib>
#include <vector>
#include <ncurses.h>
#include <stack>

const int ROW = 6;
const int COL = 7;

class ConnectFour
{
private:
    int board[ROW][COL]; // 0 = empty, 1 = player 1, 2 = player 2
    int currentPlayer;
    WINDOW *boardWindow;
    WINDOW *messageWindow;
    std::stack<int> moves;

public:
    ConnectFour()
    {
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                board[row][col] = 0;
            }
        }
        currentPlayer = 1;

        start_color();

        init_pair(1, COLOR_BLACK, COLOR_YELLOW); // Player 1
        init_pair(2, COLOR_BLACK, COLOR_RED);    // Player 2
        init_pair(3, COLOR_BLACK, COLOR_WHITE);  // Empty game board

        boardWindow = newwin(ROW * 2 + 1, COL * 4 + 1, 0, 0);
        box(boardWindow, 0, 0);
        wattron(boardWindow, COLOR_PAIR(3));
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, " ");
            }
        }
        wattroff(boardWindow, COLOR_PAIR(1));
        wrefresh(boardWindow);

        messageWindow = newwin(3, COL * 4 + 1, ROW * 2, 0);
        wrefresh(messageWindow);
        box(messageWindow, 0, 0);
        mvwprintw(messageWindow, 1, 1, "Player 1's turn (X)");
        wrefresh(messageWindow);
    }
    bool is_board_full()
    {
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                if (board[row][col] == 0)
                {
                    return false;
                }
            }
        }
        return true;
    }
    void draw_board()
{
    wclear(boardWindow);
    box(boardWindow, 0, 0);

    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            if (board[row][col] == 0)
            {
                wattron(boardWindow, COLOR_PAIR(3));
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, " ");
                wattroff(boardWindow, COLOR_PAIR(3));
            }
            else if (board[row][col] == 1)
            {
                wattron(boardWindow, COLOR_PAIR(1));
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, "X");
                wattroff(boardWindow, COLOR_PAIR(1));
            }
            else if (board[row][col] == 2)
            {
                wattron(boardWindow, COLOR_PAIR(2));
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, "O");
                wattroff(boardWindow, COLOR_PAIR(2));
            }
        }
    }

    wrefresh(boardWindow);
}

bool undoMove()
{
    if (moves.empty())
    {
        return false;
    }

    int col = moves.top();
    moves.pop();

    for (int row = 0; row < ROW; row++) // Search from the top of the board
    {
        if (board[row][col] != 0)
        {
            board[row][col] = 0;
            currentPlayer = currentPlayer == 1 ? 2 : 1; // switch player
            draw_board();
            return true;
        }
    }

    return false;
}

    void reset()
    {
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                board[row][col] = 0;
            }
        }
        currentPlayer = 1;

        wclear(boardWindow);
        box(boardWindow, 0, 0);
        wattron(boardWindow, COLOR_PAIR(3));
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, " ");
            }
        }
        while (!moves.empty())
        { // Clears the undo move stack
            moves.pop();
        }

        wattroff(boardWindow, COLOR_PAIR(1));
        wrefresh(boardWindow);

        wclear(messageWindow);
        box(messageWindow, 0, 0);
        wattron(messageWindow, COLOR_PAIR(1));
        mvwprintw(messageWindow, 1, 1, "Player 1's turn (X)");
        wattroff(messageWindow, COLOR_PAIR(1));
        wrefresh(messageWindow);
    }
    void play()
    {
        bool playAgain = true;
        while (playAgain)
        {
            reset();
            erase();
            refresh();
            draw_box();
            mvwprintw(messageWindow, 1, 1, "Player 1's turn (X)");
            wrefresh(messageWindow);

            while (1)
            {
                switch (getch())
                {
                case '1':
                    makeMove(0);
                    break;
                case '2':
                    makeMove(1);
                    break;
                case '3':
                    makeMove(2);
                    break;
                case '4':
                    makeMove(3);
                    break;
                case '5':
                    makeMove(4);
                    break;
                case '6':
                    makeMove(5);
                    break;
                case '7':
                    makeMove(6);
                    break;
                case 'u': // undo move on 'u' key press
                    if (undoMove())
                    {
                        wmove(messageWindow, 1, 0);
                        wclrtoeol(messageWindow);
                        mvwprintw(messageWindow, 1, 1, "Player %d's turn (%s)", currentPlayer, currentPlayer == 1 ? "X" : "O");
                        wrefresh(messageWindow);
                    }
                    break;
                case 'q':
                case 'Q':
                    erase();
                    mvprintw(10, 21, "Congratulations! You are a quitter!");
                    refresh();
                    getch();
                    playAgain = false; // set flag to false to exit outer loop
                }

                if (!playAgain)
                {
                    break;
                }
                wrefresh(boardWindow);
                wrefresh(messageWindow);
                if (checkForWin(1))
                {
                    wmove(messageWindow, 1, 0); // move cursor to beginning of line 1
                    wclrtoeol(messageWindow);   // clear line 1
                    mvwprintw(messageWindow, 1, 1, "Player 1 (X) wins!");
                    mvwprintw(messageWindow, 2, 1, "Play again? (y/n)");
                    wrefresh(messageWindow);
                    if (getch() == 'y')
                    {
                        reset();
                        break; // break out of inner loop to play again
                    }
                    else
                    {
                        erase();
                        mvprintw(10, 31, "Thanks for playing!");
                        refresh();
                        getch();
                        playAgain = false; // set flag to false to exit outer loop
                        break;
                    }
                }
                else if (checkForWin(2))
                {
                    wmove(messageWindow, 1, 0); // move cursor to beginning of line 1
                    wclrtoeol(messageWindow);   // clear line 1
                    mvwprintw(messageWindow, 1, 1, "Player 2 (O) wins!");
                    mvwprintw(messageWindow, 2, 1, "Play again? (y/n)");
                    wrefresh(messageWindow);
                    if (getch() == 'y')
                    {
                        reset();
                        break; // break out of inner loop to play again
                    }
                    else
                    {
                        erase();
                        mvprintw(10, 31, "Thanks for playing!");
                        refresh();
                        getch();
                        playAgain = false; // set flag to false to exit outer loop
                        break;
                    }
                }
                else if (is_board_full())
                {
                    wmove(messageWindow, 1, 0); // move cursor to beginning of line 1
                    wclrtoeol(messageWindow);   // clear line 1
                    mvwprintw(messageWindow, 1, 1, "It is a tie!");
                    mvwprintw(messageWindow, 2, 1, "Play again? (y/n)");
                    wrefresh(messageWindow);
                    if (getch() == 'y')
                    {
                        reset();
                        break; // break out of inner loop to play again
                    }
                    else
                    {
                        erase();
                        mvprintw(10, 31, "Thanks for playing!");
                        refresh();
                        getch();
                        playAgain = false; // set flag to false to exit outer loop
                        break;
                    }
                }
            }
        }
    }

    void draw_box()
    {
        box(boardWindow, 0, 0);
        wrefresh(boardWindow);
    }

    void makeMove(int col)
    {
        draw_box();
        int row = -1;
        for (int r = 0; r < ROW; r++)
        {
            if (board[r][col] == 0)
            {
                row = r;
            }
        }
        if (row == -1)
        {
            mvwprintw(messageWindow, 1, 1, "Column is full. Try again.");
            wrefresh(messageWindow);
        }
        else
        {
            board[row][col] = currentPlayer;
            if (currentPlayer == 1)
            {
                wattron(boardWindow, COLOR_PAIR(1));
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, "X");
            }
            else
            {
                wattron(boardWindow, COLOR_PAIR(2));
                mvwprintw(boardWindow, row * 2 + 1, col * 4 + 2, "O");
            }
            wattroff(boardWindow, COLOR_PAIR(1));
            wattroff(boardWindow, COLOR_PAIR(2));

            currentPlayer = (currentPlayer == 1) ? 2 : 1;
            mvwprintw(messageWindow, 1, 1, "Player %d's turn (%c)", currentPlayer, (currentPlayer == 1) ? 'X' : 'O');
            wrefresh(messageWindow);
            moves.push(col);
        }
    }

    bool checkForWin(int player)
    {
        // Check rows
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL - 3; col++)
            {
                if (board[row][col] == player && board[row][col + 1] == player && board[row][col + 2] == player && board[row][col + 3] == player)
                {
                    return true;
                }
            }
        }

        // Check columns
        for (int row = 0; row < ROW - 3; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                if (board[row][col] == player && board[row + 1][col] == player && board[row + 2][col] == player && board[row + 3][col] == player)
                {
                    return true;
                }
            }
        }

        // Check diagonal (top-left to bottom-right)
        for (int row = 0; row < ROW - 3; row++)
        {
            for (int col = 0; col < COL - 3; col++)
            {
                if (board[row][col] == player && board[row + 1][col + 1] == player && board[row + 2][col + 2] == player && board[row + 3][col + 3] == player)
                {
                    return true;
                }
            }
        }

        // Check diagonal (top-right to bottom-left)
        for (int row = 0; row < ROW - 3; row++)
        {
            for (int col = 3; col < COL; col++)
            {
                if (board[row][col] == player && board[row + 1][col - 1] == player && board[row + 2][col - 2] == player && board[row + 3][col - 3] == player)
                {
                    return true;
                }
            }
        }

        return false;
    }

    ~ConnectFour()
    {
        delwin(boardWindow);   // Deletes the board window
        delwin(messageWindow); // Deletes the message window
    }
};

void io_init_terminal(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

int main(int argc, char *argv[])
{

    io_init_terminal();
    ConnectFour game;

    mvprintw(5, 30, "Welcome to Connect 4");
    mvprintw(7, 10, "Press a number to place your piece in the corresponding column");
    getch();

    game.play(); // Main driver of the game

    endwin();

    return 0;
}
