package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"time"
)

// Генерация случайного числа в промежутке [a; b]
func generateRandomInt(a, b int) int {
	return rand.Intn(b-a+1) + a
}

// Инициализация матрицы NxM со случайными числами из промежутка [a; b]
func initializeMatrix(a, b, n, m int) [][]int {
	matrix := make([][]int, n)
	for i := range matrix {
		matrix[i] = make([]int, m)
		for j := range matrix[i] {
			matrix[i][j] = generateRandomInt(a, b)
			fmt.Printf("%d ", matrix[i][j])
		}
		fmt.Println()
	}
	return matrix
}

// Проверка на наличие цифр 5 и 7
func containsNumbers(elem int) bool {
	if elem == 0 {
		return false
	}
	for elem > 0 {
		digit := elem % 10
		if digit == 5 || digit == 7 {
			return true
		}
		elem /= 10
	}
	return false
}

// Задание 1
func func1(n int) {
	if n%2 == 1 {
		fmt.Fprintln(os.Stderr, "Введенное число должно быть четным")
		os.Exit(1)
	}

	// Исправлено: теперь явно передаем n и для количества столбцов
	matrix := initializeMatrix(100, 200, n, n)

	// Сумма элементов строк и столбцов
	for i := 0; i < n; i++ {
		lineSum, columnSum := 0, 0
		for j := 0; j < n; j++ {
			lineSum += matrix[i][j]
			columnSum += matrix[j][i]
		}
		fmt.Printf("Строка №%d: %d;\tСтолбец №%d: %d\n", i+1, lineSum, i+1, columnSum)
	}
}

// Задание 2
func func2(n, m int) []int {
	matrix := initializeMatrix(1000, 3000, n, m)
	longestSeq := []int{}

	for i := 0; i < n; i++ {
		tempSeq := []int{}
		for _, elem := range matrix[i] {
			if !containsNumbers(elem) {
				tempSeq = append(tempSeq, elem)
			} else {
				if len(longestSeq) < len(tempSeq) {
					longestSeq = tempSeq
				}
				tempSeq = []int{}
			}
		}
		// Проверяем последовательность после обработки строки
		if len(longestSeq) < len(tempSeq) {
			longestSeq = tempSeq
		}
	}

	return longestSeq
}

// Задание 3 - Клеточный автомат Джона Конвея
func func3() {
	cmd := exec.Command("gnome-terminal", "--", "bash", "-c", "../conway/conway; exec bash")
	err := cmd.Run()
	if err != nil {
		fmt.Println("Ошибка при запуске клеточного автомата:", err)
	}
}

func main() {
	rand.Seed(time.Now().UnixNano())

	fmt.Println("Доступные функции:")
	fmt.Println("1. Сумма каждой строки и столбца квадратичной матрицы")
	fmt.Println("2. Массив последовательности с числами без цифр 5 и 7")
	fmt.Println("3. Клеточный автомат Джона Конвея")
	fmt.Print("Введите номер функции: ")

	var choice, n, m int
	_, err := fmt.Scan(&choice)
	if err != nil {
		fmt.Println("Ошибка ввода:", err)
		return
	}

	switch choice {
	case 1:
		fmt.Print("Введите размерность массива (обязательно четное!): ")
		_, err := fmt.Scan(&n)
		if err != nil {
			fmt.Println("Ошибка ввода:", err)
			return
		}
		func1(n)

	case 2:
		fmt.Print("Введите значения M и N для инициализации массива MxN: ")
		_, err := fmt.Scan(&m, &n)
		if err != nil {
			fmt.Println("Ошибка ввода:", err)
			return
		}
		result := func2(n, m)
		fmt.Print("Найденная последовательность: ")
		for _, elem := range result {
			fmt.Printf("%d ", elem)
		}
		fmt.Println()

	case 3:
		func3()

	default:
		fmt.Println("Неизвестный номер")
	}
}

