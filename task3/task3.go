package main

import (
	"fmt"
	"math"
)

const (
	matrixSize = 4
	eps        = 1e-3
)

type (
	matrix []vector
	vector []float64
)

var (
	a = matrix{
		{1.25, -0.04, 0.21, -18},
		{0.25, -1.23, -1.14, -0.09},
		{-0.21, -1.14, 0.8, -0.13},
		{0.15, -1.31, 0.06, -1.09},
	}

	b = vector{
		-1.24, -1.09, 2.56, 1.25,
	}

	aCanon = matrix{
		{-0.092, 0.032, -0.168, 14.4},
		{0.886, 0.203, -0.927, -0.073},
		{3.2, 0.263, 1.425, 0.163},
		{-1.147, 0.138, -1.202, 0.055},
	}
)

func (A matrix) multiplyVector(B vector) vector {
	n := len(A)
	R := make(vector, n)
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			R[i] += A[i][j] * B[j]
		}
	}
	return R
}

func (A vector) addVector(B vector) vector {
	n := len(A)
	R := make(vector, n)
	for i := 0; i < n; i++ {
		R[i] = A[i] + B[i]
	}
	return R
}

func (mtx matrix) String() string {
	s := ""
	for _, vec := range mtx {
		for _, elem := range vec {
			s += fmt.Sprintf("%10.3f ", elem)
		}
		s += "\n"
	}
	return s
}

func (vec vector) String() string {
	s := ""
	for _, elem := range vec {
		s += fmt.Sprintf("%5.3f ", elem)
	}
	return s
}

func sum(L, U matrix, i, j int) float64 {
	result := 0.0
	limit := i
	if j < i {
		limit = j
	}

	for k := 0; k < limit; k++ {
		result += L[i][k] * U[k][j]
	}
	return result
}

func lu(A matrix) (matrix, matrix) {
	L := make(matrix, matrixSize)
	U := make(matrix, matrixSize)
	for i := 0; i < matrixSize; i++ {
		L[i] = make(vector, matrixSize)
		U[i] = make(vector, matrixSize)
		L[i][i] = 1.0
	}

	for i := 0; i < matrixSize; i++ {
		for j := 0; j < matrixSize; j++ {
			if i <= j {
				U[i][j] = A[i][j] - sum(L, U, i, j)
			} else {
				L[i][j] = (A[i][j] - sum(L, U, i, j)) / U[j][j]
			}
		}
	}
	return L, U
}

func solveLyB(L matrix, B vector) vector {
	y := make(vector, matrixSize)
	for i := 0; i < matrixSize; i++ {
		y[i] = B[i]
		for j := 0; j < i; j++ {
			y[i] -= L[i][j] * y[j]
		}
		y[i] /= L[i][i]
	}
	return y
}

func solveUxy(U matrix, y vector) vector {
	x := make(vector, matrixSize)
	for i := matrixSize - 1; i >= 0; i-- {
		sum := 0.0
		for j := i + 1; j < matrixSize; j++ {
			sum += U[i][j] * x[j]
		}
		x[i] = (y[i] - sum) / U[i][i]
	}
	return x
}

func simpleIteration(A matrix, B vector, limit int) vector {
	x := make(vector, matrixSize)
	prev := make(vector, matrixSize)
	f := make(vector, matrixSize)
	C := make(matrix, matrixSize)
	for i := 0; i < matrixSize; i++ {
		C[i] = make(vector, matrixSize)
	}

	for i := 0; i < matrixSize; i++ {
		f[i] = B[i] / A[i][i]
		for j := 0; j < matrixSize; j++ {
			if i != j {
				C[i][j] = -A[i][j] / A[i][i]
			}
		}
	}

	fmt.Println("\nИтерации метода простой итерации (ε = 1e-3):")
	fmt.Print("N\t")
	for i := 0; i < matrixSize; i++ {
		fmt.Printf("\tx%d\t\t", i+1)
	}
	fmt.Println("\tεn")

	k := 0
	for k < limit {
		copy(prev, x)
		maxDiff := 0.0

		for i := 0; i < matrixSize; i++ {
			x[i] = f[i]
			for j := 0; j < matrixSize; j++ {
				x[i] += C[i][j] * prev[j]
			}
			maxDiff = math.Max(maxDiff, math.Abs(x[i]-prev[i]))
		}

		fmt.Printf("%d\t", k)
		for _, xi := range x {
			fmt.Printf("%f\t", xi)
		}
		fmt.Printf("%f\n", maxDiff)

		if maxDiff < eps {
			break
		}
		k++
	}

	return x
}

func main() {
	L, U := lu(a)

	fmt.Println("Матрица A:\n", a)
	fmt.Println("Матрица L:\n", L)
	fmt.Println("Матрица U:\n", U)

	ySol := solveLyB(L, b)
	xSol := solveUxy(U, ySol)
	fmt.Println("y =", ySol)
	fmt.Println("x =", xSol)

	result := simpleIteration(aCanon, b, 15)
	fmt.Println(result)
}

