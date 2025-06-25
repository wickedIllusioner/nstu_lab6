package main

import (
	"encoding/hex"
	"fmt"
	"io"
	"log"
	"math/rand"
	"os"
	"strconv"
	"time"
)

type State [][]byte

var (
	sbox = []byte{
		0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
		0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
		0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
		0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
		0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
		0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
		0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
		0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
		0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
		0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
		0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
		0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
		0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
		0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
		0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
		0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16,
	}

	rcon = []byte{
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36,
	}
)

func printBlock(state State) {
	for i := 0; i < 4; i++ {
		for j := 0; j < 4; j++ {
			fmt.Printf("%02x ", state[i][j])
		}
		fmt.Println()
	}
}

func logStateToFile(state State, logFile *os.File, stage string) {
	if logFile == nil {
		return
	}

	logFile.WriteString(fmt.Sprintf("=== %s ===\n", stage))
	for i := 0; i < 4; i++ {
		for j := 0; j < 4; j++ {
			logFile.WriteString(fmt.Sprintf("%02x ", state[i][j]))
		}
		logFile.WriteString("\n")
	}
	logFile.WriteString("\n")
}

func gMul(a, b byte) byte {
	var p byte = 0
	var hiBitSet byte

	for i := 0; i < 8; i++ {
		if (b & 1) != 0 {
			p ^= a
		}

		hiBitSet = a & 0x80
		a <<= 1
		if hiBitSet != 0 {
			a ^= 0x1B
		}
		b >>= 1
	}
	return p
}

func toState(block []byte) State {
	state := make(State, 4)
	for i := 0; i < 4; i++ {
		state[i] = make([]byte, 4)
		for j := 0; j < 4; j++ {
			state[i][j] = block[i+4*j]
		}
	}
	return state
}

func fromState(state State) []byte {
	block := make([]byte, 16)
	for i := 0; i < 4; i++ {
		for j := 0; j < 4; j++ {
			block[i+4*j] = state[i][j]
		}
	}
	return block
}

func keyExpansion(key []byte) []State {
	expandedKey := make([]byte, 176)
	temp := make([]byte, 4)

	copy(expandedKey[:16], key)

	for i := 16; i < 176; i += 4 {
		copy(temp, expandedKey[i-4:i])

		if i%16 == 0 {
			tmp := temp[0]
			temp[0] = temp[1]
			temp[1] = temp[2]
			temp[2] = temp[3]
			temp[3] = tmp

			temp[0] = sbox[temp[0]]
			temp[1] = sbox[temp[1]]
			temp[2] = sbox[temp[2]]
			temp[3] = sbox[temp[3]]

			temp[0] ^= rcon[(i/16)-1]
		}

		expandedKey[i] = expandedKey[i-16] ^ temp[0]
		expandedKey[i+1] = expandedKey[i-15] ^ temp[1]
		expandedKey[i+2] = expandedKey[i-14] ^ temp[2]
		expandedKey[i+3] = expandedKey[i-13] ^ temp[3]
	}

	roundKeys := make([]State, 11)
	for round := 0; round <= 10; round++ {
		roundKeyBytes := expandedKey[round*16 : (round+1)*16]
		roundKeys[round] = toState(roundKeyBytes)
	}
	return roundKeys
}

func subBytes(state State) {
	for i := 0; i < 4; i++ {
		for j := 0; j < 4; j++ {
			state[i][j] = sbox[state[i][j]]
		}
	}
}

func shiftRows(state State) {
	temp := state[1][0]
	state[1][0] = state[1][1]
	state[1][1] = state[1][2]
	state[1][2] = state[1][3]
	state[1][3] = temp

	state[2][0], state[2][2] = state[2][2], state[2][0]
	state[2][1], state[2][3] = state[2][3], state[2][1]

	temp = state[3][3]
	state[3][3] = state[3][2]
	state[3][2] = state[3][1]
	state[3][1] = state[3][0]
	state[3][0] = temp
}

func mixColumns(state State) {
	tmp := make(State, 4)
	for i := 0; i < 4; i++ {
		tmp[i] = make([]byte, 4)
	}

	for j := 0; j < 4; j++ {
		tmp[0][j] = gMul(0x02, state[0][j]) ^ gMul(0x03, state[1][j]) ^ state[2][j] ^ state[3][j]
		tmp[1][j] = state[0][j] ^ gMul(0x02, state[1][j]) ^ gMul(0x03, state[2][j]) ^ state[3][j]
		tmp[2][j] = state[0][j] ^ state[1][j] ^ gMul(0x02, state[2][j]) ^ gMul(0x03, state[3][j])
		tmp[3][j] = gMul(0x03, state[0][j]) ^ state[1][j] ^ state[2][j] ^ gMul(0x02, state[3][j])
	}

	for i := 0; i < 4; i++ {
		copy(state[i], tmp[i])
	}
}

func addRoundKey(state State, roundKey State) {
	for i := 0; i < 4; i++ {
		for j := 0; j < 4; j++ {
			state[i][j] ^= roundKey[i][j]
		}
	}
}

func encryptBlock(block []byte, key []byte) {
	logFile, err := os.OpenFile("aes_state_log.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Println("Error opening log file:", err)
	}
	defer func() {
		if logFile != nil {
			logFile.Close()
		}
	}()

	state := toState(block)
	roundKeys := keyExpansion(key)

	logStateToFile(state, logFile, "Начальное состояние")
	logStateToFile(roundKeys[0], logFile, "Ключ раунда 0")

	addRoundKey(state, roundKeys[0])
	logStateToFile(state, logFile, "AddRoundKey раунда 0")

	for round := 1; round < 10; round++ {
		subBytes(state)
		logStateToFile(state, logFile, "SubBytes раунда "+strconv.Itoa(round))

		shiftRows(state)
		logStateToFile(state, logFile, "ShiftRows раунда "+strconv.Itoa(round))

		mixColumns(state)
		logStateToFile(state, logFile, "MixColumns раунда "+strconv.Itoa(round))

		addRoundKey(state, roundKeys[round])
		logStateToFile(roundKeys[round], logFile, "Ключ раунда "+strconv.Itoa(round))
		logStateToFile(state, logFile, "AddRoundKey раунда "+strconv.Itoa(round))
	}

	subBytes(state)
	logStateToFile(state, logFile, "SubBytes финального раунда")

	shiftRows(state)
	logStateToFile(state, logFile, "ShiftRows финального раунда")

	addRoundKey(state, roundKeys[10])
	logStateToFile(roundKeys[10], logFile, "Ключ финального раунда")
	logStateToFile(state, logFile, "AddRoundKey финального раунда")

	copy(block, fromState(state))
}

func generateRandomBytes(buffer []byte) {
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	for i := range buffer {
		buffer[i] = byte(r.Intn(256))
	}
}

func hexStringToBytes(s string) ([]byte, error) {
	if len(s)%2 != 0 {
		return nil, fmt.Errorf("hex string must have even length")
	}
	return hex.DecodeString(s)
}

func bytesToHexString(b []byte) string {
	return hex.EncodeToString(b)
}

func encrypt(in io.Reader, out io.Writer, key []byte, iv []byte) error {
	feedback := make([]byte, 16)
	copy(feedback, iv)

	buffer := make([]byte, 16)
	for {
		bytesRead, err := in.Read(buffer)
		if err != nil {
			if err == io.EOF {
				break
			}
			return fmt.Errorf("error reading input file: %w", err)
		}

		plaintext := make([]byte, bytesRead)
		copy(plaintext, buffer[:bytesRead])

		encryptedFeedback := make([]byte, 16)
		copy(encryptedFeedback, feedback)
		encryptBlock(encryptedFeedback, key)

		ciphertext := make([]byte, bytesRead)
		for i := 0; i < bytesRead; i++ {
			ciphertext[i] = plaintext[i] ^ encryptedFeedback[i]
		}

		_, err = out.Write(ciphertext)
		if err != nil {
			return fmt.Errorf("error writing output file: %w", err)
		}

		if bytesRead == 16 {
			copy(feedback, ciphertext)
		} else {
			copy(feedback, ciphertext)
			for i := bytesRead; i < 16; i++ {
				feedback[i] = 0 // Pad with zeros for shorter last block
			}
		}
	}
	return nil
}

func decrypt(in io.Reader, out io.Writer, key []byte, iv []byte) error {
	feedback := make([]byte, 16)
	copy(feedback, iv)

	buffer := make([]byte, 16)
	for {
		bytesRead, err := in.Read(buffer)
		if err != nil {
			if err == io.EOF {
				break
			}
			return fmt.Errorf("error reading input file: %w", err)
		}

		ciphertext := make([]byte, bytesRead)
		copy(ciphertext, buffer[:bytesRead])

		encryptedFeedback := make([]byte, 16)
		copy(encryptedFeedback, feedback)
		encryptBlock(encryptedFeedback, key)

		plaintext := make([]byte, bytesRead)
		for i := 0; i < bytesRead; i++ {
			plaintext[i] = ciphertext[i] ^ encryptedFeedback[i]
		}

		_, err = out.Write(plaintext)
		if err != nil {
			return fmt.Errorf("error writing output file: %w", err)
		}

		if bytesRead == 16 {
			copy(feedback, ciphertext)
		} else {
			copy(feedback, ciphertext)
			for i := bytesRead; i < 16; i++ {
				feedback[i] = 0
			}
		}
	}
	return nil
}

func ivHandler(iv *[]byte) {
	fmt.Print("Введите 32-значное значение IV: ")
	var ivStr string
	fmt.Scan(&ivStr)

	if len(ivStr) != 32 {
		log.Fatalf("Длина IV не совпадает с 32 символами!\n")
	}

	parsedIV, err := hexStringToBytes(ivStr)
	if err != nil {
		log.Fatalf("Ошибка парсинга IV: %v\n", err)
	}
	*iv = parsedIV
}

func keyHandler(key *[]byte) {
	fmt.Print("Введите 32-значное значение ключа: ")
	var keyStr string
	fmt.Scan(&keyStr)

	if len(keyStr) != 32 {
		log.Fatalf("Длина ключа не совпадает с 32 символами!\n")
	}

	parsedKey, err := hexStringToBytes(keyStr)
	if err != nil {
		log.Fatalf("Ошибка парсинга ключа: %v\n", err)
	}
	*key = parsedKey
}

func main() {
	clearLog, err := os.OpenFile("aes_state_log.txt", os.O_TRUNC|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatalf("Не удалось очистить файл aes_state_log.txt: %v\n", err)
	}
	clearLog.Close()

	var choice int
	var inputFile, outputFile string
	key := make([]byte, 16)
	iv := make([]byte, 16)

	fmt.Println("AES-128 CFB")
	fmt.Println("1. Зашифровать файл")
	fmt.Println("2. Расшифровать файл")
	fmt.Print("Ваш выбор: ")
	fmt.Scan(&choice)

	if choice != 1 && choice != 2 {
		fmt.Println("Неверное значение. Попробуйте снова")
		os.Exit(1)
	}

	fmt.Print("Файл для шифровки/расшифровки: ")
	fmt.Scan(&inputFile)
	fmt.Print("Файл для результата операции: ")
	fmt.Scan(&outputFile)

	inFile, err := os.Open(inputFile)
	if err != nil {
		fmt.Printf("Не удалось открыть файл ввода: %v\n", err)
		os.Exit(1)
	}
	defer inFile.Close()

	outFile, err := os.Create(outputFile)
	if err != nil {
		fmt.Printf("Не удалось открыть файл вывода: %v\n", err)
		os.Exit(1)
	}
	defer outFile.Close()

	if choice == 1 {
		keyFile, err := os.OpenFile("keys.txt", os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
		if err != nil {
			fmt.Printf("Не удалось внести данные ключей в файл: %v\n", err)
			os.Exit(1)
		}
		defer keyFile.Close()

		generateRandomBytes(key)
		fmt.Printf("Случайный ключ: %s\n", bytesToHexString(key))

		generateRandomBytes(iv)
		fmt.Printf("Случайный IV: %s\n", bytesToHexString(iv))

		keysToWrite := fmt.Sprintf("KEY: %s\nIV: %s", bytesToHexString(key), bytesToHexString(iv))
		_, err = keyFile.WriteString(keysToWrite)
		if err != nil {
			fmt.Printf("Не удалось записать ключи в файл: %v\n", err)
			os.Exit(1)
		}

		roundKeys := keyExpansion(key)
		fmt.Println("\nКлючи раундов:")
		for i, rk := range roundKeys {
			fmt.Printf("Раунд %2d: ", i)
			for j := 0; j < 4; j++ {
				for k := 0; k < 4; k++ {
					fmt.Printf("%02x", rk[k][j])
				}
			}
			fmt.Println()
		}

		err = encrypt(inFile, outFile, key, iv)
		if err != nil {
			fmt.Printf("Ошибка при шифровании файла: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("\nФайл успешно зашифрован")
		fmt.Println("Сгенерированные значения были записаны в файл keys.txt")
		fmt.Println("Промежуточные состояние State записаны в файл aes_state_log.txt")

	} else {
		keyHandler(&key)
		ivHandler(&iv)

		err = decrypt(inFile, outFile, key, iv)
		if err != nil {
			fmt.Printf("Ошибка при расшифровке файла: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("Файл успешно расшифрован")
	}
}

