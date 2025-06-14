#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <random>
#include <limits>

using namespace std;

const int GRID_WIDTH = 90;
const int GRID_HEIGHT = 60;
const char LIVE_CELL_SYMBOL = '*';
const char DEAD_CELL_SYMBOL = ' ';
const int GENERATION_DELAY_MS = 150;

vector<vector<bool>> gameGrid(GRID_HEIGHT, vector<bool>(GRID_WIDTH, false));

void clearScreen();
void initializeWeed();
void initializeRandom();
int countLiveNeighbors(int row, int col);
void updateGrid();
void displayGrid();


// Очистка экрана
void clearScreen() {
    system("clear");
}

// Создание фигуры "Репликатор"
void initializeWeed() {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            gameGrid[i][j] = false;
        }
    }

    // Размещение фигуры по середине автомата
    int startRow = GRID_HEIGHT / 2 - 2;
    int startCol = GRID_WIDTH / 2 - 2;

      // Середина
    gameGrid[startRow][startCol - 2] = true;
    gameGrid[startRow][startCol + 2] = true;

    gameGrid[startRow - 1][startCol - 1] = true;
    gameGrid[startRow - 1][startCol + 2] = true;

    gameGrid[startRow - 2][startCol] = true;
    gameGrid[startRow - 2][startCol + 1] = true;
    gameGrid[startRow - 2][startCol + 2] = true;

    gameGrid[startRow + 1][startCol - 2] = true;
    gameGrid[startRow + 1][startCol + 1] = true;

    gameGrid[startRow + 2][startCol] = true;
    gameGrid[startRow + 2][startCol - 1] = true;
    gameGrid[startRow + 2][startCol - 2] = true;

}

void initializeRandom() {
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution d(0.3); // Approx 30% chance of being alive

    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            gameGrid[i][j] = d(gen);
        }
    }
}

// Подсчет живых клеток
int countLiveNeighbors(int row, int col) {
    int liveNeighbors = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;

            // Handle wrap-around (toroidal grid)
            int neighborRow = (row + i + GRID_HEIGHT) % GRID_HEIGHT;
            int neighborCol = (col + j + GRID_WIDTH) % GRID_WIDTH;

            if (gameGrid[neighborRow][neighborCol]) {
                liveNeighbors++;
            }
        }
    }
    return liveNeighbors;
}

void updateGrid() {
    vector<vector<bool>> nextGrid = gameGrid; // Create a temporary grid for the next state

    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            int liveNeighbors = countLiveNeighbors(i, j);

            if (gameGrid[i][j]) { // Current cell is alive
                if (liveNeighbors < 2 || liveNeighbors > 3) {
                    nextGrid[i][j] = false; // Dies
                }
                // Else: 2 or 3 neighbors, stays alive (nextGrid[i][j] remains true)
            } else { // Current cell is dead
                if (liveNeighbors == 3 || liveNeighbors == 6) {
                    nextGrid[i][j] = true; // Becomes alive (reproduction)
                }
            }
        }
    }
    gameGrid = nextGrid; // Update the main grid
}

// Вывод клетчатого автомата
void displayGrid() {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            cout << (gameGrid[i][j] ? LIVE_CELL_SYMBOL : DEAD_CELL_SYMBOL) << " ";
        }
        cout << endl;
    }
}

int main() {
    char choice;

    do {
        clearScreen();
        cout << "Автомат Джона Конвея" << endl;
        cout << "---------------------" << endl;
        cout << "Выберите режим:" << endl;
        cout << "1. Репликатор" << endl;
        cout << "2. Развитие колоний клеток в случайном порядке" << endl;
        cout << "Ваш выбор: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == '1') {
            initializeWeed();
        } else if (choice == '2') {
            initializeRandom();
        } else {
            cout << "Неверное значение" << endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
    } while (choice != '1' && choice != '2');

    cout << "Симуляция начнется после нажатия клавиши Enter. Для остановки - Ctrl+C" << endl;
    cin.get();

    while (true) {
        clearScreen();
        displayGrid();
        updateGrid();
        this_thread::sleep_for(chrono::milliseconds(GENERATION_DELAY_MS));
    }

    return 0;
}
