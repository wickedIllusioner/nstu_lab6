#include <iostream>
#include <vector>
#include <random>

using namespace std;

int GenerateRandomInt() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100, 200);
    return dis(gen);
}

int main() {
    vector<vector<int>> matrix;
    int n {};
    cin >> n;

    // Инициализация массива
    if (n % 2 == 0) {
        matrix.resize(n);
        for (int i = 0; i < n; ++i) {
            matrix[i].resize(n);
            for (int j = 0; j < n; ++j) {
                matrix[i][j] = GenerateRandomInt();
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Сумма элементов строк и столбцов
    for (int i = 0; i < n; ++i) {
        int LineSum {}, ColumnSum {};
        for (int j = 0; j < n; ++j) {
            LineSum += matrix[i][j];
            ColumnSum += matrix[j][i];
        }
        printf("Строка №%d: %d;\tСтолбец №%d: %d\n", i+1, LineSum, i+1, ColumnSum);
    }

    return 0;
}
