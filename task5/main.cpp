#include <iostream>
#include <vector>
#include <climits>
#include <random>
#include <algorithm>

using namespace std;
using Chessboard = vector<vector<int>>;

const int INF = INT_MAX;

// Функция для переворота монеты
void flip(int& coin) {
    coin = 1 - coin;
}

// Функция для проверки, является ли доска шахматной с заданным начальным значением
int checkChessboard(const Chessboard& board, int start) {
    int n = board.size();
    if (n == 0) return 0;
    int m = board[0].size();

    // Создаем копию доски для модификации
    Chessboard temp = board;
    int flips = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            // Ожидаемое значение в клетке (i,j)
            int expected = (i + j + start) % 2;

            // Если текущее значение не совпадает с ожидаемым
            if (temp[i][j] != expected) {
                // Пытаемся перевернуть с правым соседом
                if (j + 1 < m) {
                    flip(temp[i][j]);
                    flip(temp[i][j+1]);
                    flips++;
                }
                // Иначе пытаемся перевернуть с нижним соседом
                else if (i + 1 < n) {
                    flip(temp[i][j]);
                    flip(temp[i+1][j]);
                    flips++;
                }
                // Если не можем перевернуть (последняя клетка)
                else {
                    return INF;
                }
            }
        }
    }

    // Проверяем, получилась ли шахматная доска
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (temp[i][j] != (i + j + start) % 2) {
                return INF;
            }
        }
    }

    return flips;
}

// Функция для нахождения минимального числа переворотов
int minFlipsToChessboard(Chessboard& board) {
    int n = board.size();
    if (n == 0) return 0;

    // Проверяем оба возможных шахматных варианта
    int option1 = checkChessboard(board, 0);
    int option2 = checkChessboard(board, 1);

    int result = min(option1, option2);

    return (result == INF) ? -1 : result;
}

// Функция для генерации случайной доски
Chessboard generateRandomBoard(int n, int m) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    Chessboard board(n, vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            board[i][j] = dis(gen);
        }
    }
    return board;
}

// Функция для вывода доски
void printBoard(const Chessboard& board) {
    for (const auto& row : board) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

int main() {
    int n, m;
    cout << "Введите размеры доски N и M: ";
    cin >> n >> m;

    // Генерируем случайную доску
    Chessboard board = generateRandomBoard(n, m);

    cout << "Сгенерированная доска:" << endl;
    printBoard(board);

    // Находим минимальное число переворотов
    int result = minFlipsToChessboard(board);

    if (result == -1) {
        cout << "Невозможно преобразовать доску в шахматный порядок." << endl;
    } else {
        cout << "Минимальное количество переворотов: " << result << endl;
    }

    return 0;
}
