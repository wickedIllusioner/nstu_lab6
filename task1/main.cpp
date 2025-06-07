#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <random>

using namespace std;

// Генерация случайного в промежутке [a; b]
int generateRandomInt(int a, int b) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(a, b);
    return dis(gen);
}

// Инициализация массива NxM со случайными числами из промежутка [a; b]
vector<vector<int>> initializeMatrix(const int a, const int b, const int n, int m = 0) {
    if (m == 0) m = n;
    vector<vector<int>> matrix;
    matrix.resize(n);
    for (int i = 0; i < n; ++i) {
        matrix[i].resize(n);
        for (int j = 0; j < m; ++j) {
            matrix[i][j] = generateRandomInt(a, b);
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    return matrix;
}

// Проверка на наличие цифр 5 и 7
bool containsNumbers(int elem) {
    if (elem == 0) return false;
    while (elem > 0) {
        int digit = elem % 10;
        if (digit == 5 || digit == 7)
            return true;
        elem /= 10;
    }
    return false;
}

// Задание 1
void func1(int n) {
    if (n % 2 == 1) {
        cerr << "Введенное число должно быть четным" << endl;
        abort();
    }

    vector<vector<int>> matrix = initializeMatrix(100, 200, n);

    // Сумма элементов строк и столбцов
    for (int i = 0; i < n; ++i) {
        int LineSum {}, ColumnSum {};
        for (int j = 0; j < n; ++j) {
            LineSum += matrix[i][j];
            ColumnSum += matrix[j][i];
        }
        printf("Строка №%d: %d;\tСтолбец №%d: %d\n", i+1, LineSum, i+1, ColumnSum);
    }
}

// Задание 2
vector<int> func2(int n, int m) {
    vector<vector<int>> matrix = initializeMatrix(1000, 3000, n, m);
    vector<int> longestSeq;
    // longestSeq.resize(m);

    for (int i = 0; i < n; ++i) {
        vector<int> tempSeq;
        for (int elem : matrix[i]) {
            if (!containsNumbers(elem))
                tempSeq.push_back(elem);
            else {
                if (longestSeq.size() < tempSeq.size())
                    longestSeq = tempSeq;
                tempSeq.clear();
            }
        }
    }

    return longestSeq;
}





int main() {
    cout << "Доступные функции: \n" << "1. Сумма каждой строки и столбца квадратичной матрицы" << endl;
    cout << "2. Массив последовательности с числами без цифр 5 и 7" << endl;
    cout << "3. Клеточный автомат Джона Конвея (сорняк)" << endl;
    cout << "Введите номер функции: ";

    int choice {}, n {}, m {};
    cin >> choice;
    switch (choice) {
        case 1:
            cin >> n;
            func1(n);
            break;

        case 2:
            cin >> n >> m;
            auto result = func2(n, m);
            cout << "Найденная последовательность: " ;
            for (int elem : result) {
                cout << elem << " ";
            }
            cout << endl;
            break;

        // default:
        //     cout << "Неизвестный номер" << endl;
    }


    return 0;
}
