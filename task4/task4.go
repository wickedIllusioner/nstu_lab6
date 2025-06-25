package main

import (
	"fmt"
)

func generateCodes(kCurrent int, currentCode string, letterCodes []string, K int, codeCounts map[string]int) {
	if kCurrent == K {
		codeCounts[currentCode]++
		return
	}

	for i := 0; i < len(letterCodes); i++ {
		generateCodes(kCurrent+1, currentCode+letterCodes[i], letterCodes, K, codeCounts)
	}
}

func main() {
	var N, K int
	fmt.Scan(&N, &K)

	letterCodes := make([]string, N)
	for i := 0; i < N; i++ {
		fmt.Scan(&letterCodes[i])
	}

	codeCounts := make(map[string]int)

	generateCodes(0, "", letterCodes, K, codeCounts)

	uniqueDecodableWords := 0
	for _, count := range codeCounts {
		if count == 1 {
			uniqueDecodableWords++
		}
	}

	fmt.Println(uniqueDecodableWords)
}

