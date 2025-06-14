#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;
using Matrix = vector<vector<double>>;

vector<double> operator*(vector<vector<double>>& A, vector<double>& B) {
    int n = A.size();
    vector<double> R(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            R[i] += A[i][j] * B[j];

    return R;
}

//перегрузка сложение векторов
vector<double> operator+(const vector<double>& A, const vector<double>& B) {
    int n = A.size();
    vector<double> R(n);

    for (size_t i = 0; i < n; i++)
        R[i] = A[i] + B[i];

    return R;
}



// Система линейных алгебраических уравнений (Вариант 5)
const Matrix A = {
    {1.25, -0.04, 0.21, -18},
    {0.25, -1.23, -1.14, -0.09},
    {-0.21, -1.14, 0.8, -0.13},
    {0.15, -1.31, 0.06, -1.09}
};

const vector B = {
    -1.24, -1.09, 2.56, 1.25
};

const Matrix A_CANON = {
    {-0.092,     0.032,    -0.168,    14.4},
    {0.886,     0.203,    -0.927,    -0.073},
    {3.2,       0.263,     1.425,     0.163},
    {-1.147,    0.138,    -1.202,     0.055}
};

const int MATRIX_SIZE = 4;
const double EPS = 1e-3;


// Перегрузка оператора вывода матрицы
ostream& operator<<(ostream& os, Matrix mtx) {
    for (auto vec : mtx) {
        for (double elem : vec) {
            os << setw(10) << setprecision(3) << elem << " ";
        }
        os << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, vector<double> vec) {
    for (double elem : vec) {
        os << setw(5) << setprecision(3) <<  elem << " ";
    }
    return os;
}


// Сумма элементов матриц
double Sum(const Matrix& L, const Matrix& U, int i, int j) {
    double result {};
    int limit = (i <= j ? i : j);

    for (int k = 0; k < limit; ++k) {
        result += L[i][k] * U[k][j];
    }

    return result;
}

// Получение матриц L и U
void LU(const Matrix A, Matrix& L, Matrix& U) {
    // Первый цикл
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            U[i][j] = 0;
            L[i][j] = 0;
        }
        L[i][i] = 1;
    }

    // Второй цикл
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            if (i <= j) {
                U[i][j] = A[i][j] - Sum(L, U, i, j);
            } else {
                L[i][j] = (A[i][j] - Sum(L, U, i, j)) / U[j][j];
            }
        }
    }
}

// Решение системы Ly = B
vector<double> SolveLyB(const Matrix& L, const vector<double>& B) {
    vector y(MATRIX_SIZE, 0.);

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        y[i] = B[i];
        for (int j = 0; j < i; ++j)
            y[i] -= L[i][j]*y[j];
        y[i] /= L[i][i];
    }
    return y;
}

// Решение системы Ux = y
vector<double> SolveUxy(const Matrix& U, const vector<double> y) {
    vector x(MATRIX_SIZE, 0.);

    for (int i = MATRIX_SIZE-1; i >= 0; --i) {
        double sum {};
        for (int j = i + 1; j < MATRIX_SIZE; ++j) {
            sum += U[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / U[i][i];
    }
    return x;
}


// Метод простой итерации
vector<double> SimpleIteration(const Matrix& A, const vector<double>& B) {
    // Инициализация матриц значений x0, f, C
    vector<double> x(MATRIX_SIZE, 0.), prev(MATRIX_SIZE, 0.);
    vector<double> f(MATRIX_SIZE);
    Matrix C(MATRIX_SIZE, vector<double>(MATRIX_SIZE, 0));

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        f[i] = B[i] / A[i][i];
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            if (i != j)
                C[i][j] = -A[i][j] / A[i][i];
        }
    }

    cout << "\nИтерации метода простой итерации (ε = 1e-3):\n";
    cout << "N\t";
    for (int i = 0; i < MATRIX_SIZE; ++i) cout << "\tx" << i + 1 << "\t\t";
    cout << "\tεn" << endl;

    int k = 0;
    while (true) {
        prev = x;
        double maxDiff {};

        for (int i = 0; i < MATRIX_SIZE; ++i) {
            x[i] = f[i];
            for (int j = 0; j < MATRIX_SIZE; ++j)
                x[i] += C[i][j] * prev[j];
            maxDiff = max(maxDiff, fabs(x[i] - prev[i]));
        }

        cout << k << "\t";
        for (double xi : x) cout << fixed << setprecision(6) << xi << "\t";
        cout << maxDiff << endl;

        if (maxDiff < EPS) break;
        k++;
    }

    return x;
}


int main() {
    Matrix L(MATRIX_SIZE, vector<double>(MATRIX_SIZE));
    Matrix U(MATRIX_SIZE, vector<double>(MATRIX_SIZE));
    LU(A, L, U);

    cout << "Матрица A:\n " << A << endl << endl;
    cout << "Матрица L:\n " << L<< endl << endl;
    cout << "Матрица U:\n " << U << endl << endl;

    auto y_sol = SolveLyB(L, B);
    auto x_sol = SolveUxy(U, y_sol);
    cout << "y = " << y_sol << endl;
    cout << "x = " << x_sol << endl;

    auto result = SimpleIteration(A_CANON, B);
    cout << result;

    return 0;
}
