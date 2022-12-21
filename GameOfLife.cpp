// Author: Mario Talevski
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

//Move OS defines up here to be used in different places
#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
//WINDOWS COLORS
#define COLOR_RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED)
#define COLOR_GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN)

#define COLOR_BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE)

#define COLOR_RESET SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15)

#elif defined(linux) || defined(__CYGWIN__)
#define OS_LINUX

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

#elif (defined(__APPLE__) || defined(__OSX__) || defined(__MACOS__)) && \
    defined(__MACH__)  //To ensure that we are running on a modern version of macOS
#define OS_MAC

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

#endif

#if defined(OS_WIN)
#include <windows.h>  // Use for windows
#endif

using namespace std;

const int gridSizeX = 42;
const int gridSizeY = 20;
static int totalDiedCount = 0;
static int step = 0;

void printGrid(bool gridOne[gridSizeX + 1][gridSizeY + 1]);
void updateState(bool gridOne[gridSizeX + 1][gridSizeY + 1]);
void clearScreen(void);

int main()
{
    // system( "color A" );//LGT green
    cout << COLOR_GREEN;
    clearScreen();
    bool gridOne[gridSizeX + 1][gridSizeY + 1] = {};
    int x, y, n;
    string nc;
    string start;
    string filename;
    cout << "                         THE GAME OF life - Implementation in C++" << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "Also known simply as life, " << endl;
    cout << "is a cellular automaton devised by the British mathematician John Horton Conway in 1970." << endl;
    cout << endl;
    cout << "Rules" << endl;
    cout << "The universe of the Game of life is an infinite two-dimensional orthogonal grid of square cells," << endl;
    cout << "each of which is in one of two possible states, life or dead. Every" << endl;
    cout << "cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or "
            "diagonally adjacent."
         << endl;
    cout << "At each step in time, the following transitions occur:" << endl;
    cout << "1. Any live cell with fewer than two live neighbours dies, as if caused by under-population." << endl;
    cout << "2. Any live cell with two or three live neighbours lives on to the next generation." << endl;
    cout << "3. Any live cell with more than three live neighbours dies, as if by over-population." << endl;
    cout << "4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction." << endl;
    cout << endl;
    cout << "O - living cell" << endl;
    cout << ". - dead cell" << endl;
    cout << endl;
    cout << "Enter the number of cells, or 'r' to read cells from file: ";
    cin >> nc;
    cout << endl;

    if (nc == "r") {
        while (true) {
            cout << "Enter name of file to read from: " << endl;
            cin >> filename;

            ifstream readfile(filename);
            if (readfile.is_open()) {
                string fileline, xx, yy;

                while (getline(readfile, fileline)) {
                    stringstream ss(fileline);

                    getline(ss, xx, ' ');
                    getline(ss, yy, ' ');

                    x = stoi(xx);
                    y = stoi(yy);

                    gridOne[x][y] = true;
                }
                break;
            } else {
                cout << "No such file, try again." << endl;
            }
        }
    } else {
        for (int i = 0; i < stoi(nc); i++) {
            cout << stoi(nc) << "Enter the coordinates of cell " << i + 1 << " : ";
            cin >> x >> y;
            gridOne[x][y] = true;
            printGrid(gridOne);
        }
    }
    cout << "Grid setup is done. Start the game ? (y/n)" << endl;
    printGrid(gridOne);
    cin >> start;
    if (start == "y" || start == "Y") {
        while (true) {
            updateState(gridOne);
            printGrid(gridOne);
            usleep(200000);
            clearScreen();
        }
    } else {
        cout << COLOR_RESET;
        clearScreen();
        return 0;
    }
}

void clearScreen(void)
{
// Tested and working on Ubuntu and Cygwin
#if defined(OS_WIN)
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    if (hStdOut == INVALID_HANDLE_VALUE)
        return;

    /* Get the number of cells in the current buffer */
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count);

    /* Fill the entire buffer with the current colors and attributes */
    FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);

    SetConsoleCursorPosition(hStdOut, homeCoords);

#elif defined(OS_LINUX) || defined(OS_MAC)
    cout << "\033[2J;"
         << "\033[1;1H";  // Clears screen and moves cursor to home pos on POSIX systems
#endif
}

void printGrid(bool gridOne[gridSizeX + 1][gridSizeY + 1])
{
    int aliveCount = 0;
    for (int a = 1; a < gridSizeX; a++) {
        for (int b = 1; b < gridSizeY; b++) {
            if (gridOne[a][b]) {
                cout << " O ";
                aliveCount++;
            } else {
                cout << " . ";
            }
            if (b == gridSizeY - 1) {
                cout << endl;
            }
        }
    }
    cout << "Step: " << step << ", Alive: " << aliveCount << ", Total died: " << totalDiedCount << endl;
}

void copyGrid(bool gridOne[gridSizeX + 1][gridSizeY + 1], bool gridTwo[gridSizeX + 1][gridSizeY + 1])
{
    for (int a = 0; a < gridSizeX + 1; a++) {
        for (int b = 0; b < gridSizeY + 1; b++) {
            gridTwo[a][b] = gridOne[a][b];
        }
    }
}

void updateState(bool gridOne[gridSizeX + 1][gridSizeY + 1])
{
    bool gridTwo[gridSizeX + 1][gridSizeY + 1];
    copyGrid(gridOne, gridTwo);

    for (int a = 1; a < gridSizeX; a++) {
        for (int b = 1; b < gridSizeY; b++) {
            int alive = 0;
            for (int c = -1; c < 2; c++) {
                for (int d = -1; d < 2; d++) {
                    if (!(c == 0 && d == 0)) {
                        if (gridTwo[a + c][b + d]) {
                            ++alive;
                        }
                    }
                }
            }
            if (gridTwo[a][b] && (alive == 2 || alive == 3)) {
                gridOne[a][b] = true;
            } else if (!gridTwo[a][b] && alive == 3) {
                gridOne[a][b] = true;
            } else if (gridTwo[a][b]) {
                totalDiedCount++;
                gridOne[a][b] = false;
            } else {
                gridOne[a][b] = false;
            }
        }
    }
    step++;
}
