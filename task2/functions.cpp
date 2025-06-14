#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>


using namespace std;
using State = vector<vector<unsigned char>>;

// Таблица Sbox
const vector<unsigned char> SBOX = {
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
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// Константы раундов Rcon
const vector<unsigned char> RCON = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// Вывод блока в консоль (теперь для двумерного вектора)
void PrintBlock(const State& state) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << hex << setw(2) << setfill('0')
                 << static_cast<int>(state[i][j]) << " ";
        }
        cout << endl;
    }
    cout << dec;
}

// Функция для записи состояния State в файл
void LogStateToFile(const State& state, ofstream& logFile, const string& stage) {
    if (!logFile.is_open()) return;

    logFile << "=== " << stage << " ===" << endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            logFile << hex << setw(2) << setfill('0')
                   << static_cast<int>(state[i][j]) << " ";
        }
        logFile << endl;
    }
    logFile << dec << endl;
}

// Умножение в поле Галуа (GF(2^8))
unsigned char GMul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    unsigned char hi_bit_set;

    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }

        hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1B;
        }
        b >>= 1;
    }

    return p;
}

// Преобразование одномерного вектора в двумерный
State ToState(const vector<unsigned char>& block) {
    State state(4, vector<unsigned char>(4));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = block[i + 4*j];
        }
    }
    return state;
}

// Преобразование двумерного вектора в одномерный
vector<unsigned char> FromState(const State& state) {
    vector<unsigned char> block(16);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i + 4*j] = state[i][j];
        }
    }
    return block;
}

// Получение ключей для каждого раунда
vector<State> KeyExpansion(const vector<unsigned char>& key) {
    vector<unsigned char> expandedKey(176);
    vector<unsigned char> temp(4);

    // Записываем исходный ключ
    for (int i = 0; i < 16; i++) {
        expandedKey[i] = key[i];
    }

    // Генерация для остальных раундов
    for (int i = 16; i < 176; i += 4) {
        // Read previous word
        temp[0] = expandedKey[i - 4];
        temp[1] = expandedKey[i - 3];
        temp[2] = expandedKey[i - 2];
        temp[3] = expandedKey[i - 1];

        // Perform key schedule core for each 4th word
        if (i % 16 == 0) {
            // RotWord
            unsigned char tmp = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = tmp;

            // SubWord
            temp[0] = SBOX[temp[0]];
            temp[1] = SBOX[temp[1]];
            temp[2] = SBOX[temp[2]];
            temp[3] = SBOX[temp[3]];

            // XOR с константами раундов Rcon
            temp[0] ^= RCON[(i/16) - 1];
        }

        // Операция XOR с предыдущим ключом
        expandedKey[i] = expandedKey[i - 16] ^ temp[0];
        expandedKey[i + 1] = expandedKey[i - 15] ^ temp[1];
        expandedKey[i + 2] = expandedKey[i - 14] ^ temp[2];
        expandedKey[i + 3] = expandedKey[i - 13] ^ temp[3];
    }

    // Преобразуем в вектор из 11 ключей 4x4
    vector<State> roundKeys;
    for (int round = 0; round <= 10; round++) {
        vector roundKey(expandedKey.begin() + round*16, expandedKey.begin() + (round+1)*16);
        roundKeys.push_back(ToState(roundKey));
    }

    return roundKeys;
}

// Замена каждого байта относительно таблицы SBox
void SubBytes(State& state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = SBOX[state[i][j]];
        }
    }
}

// Циклический сдвиг строк
void ShiftRows(State& state) {
    unsigned char temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    swap(state[2][0], state[2][2]);
    swap(state[2][1], state[2][3]);

    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;
}

// Смешивание столбцов с помощью матричного умножения в поле Галуа
void MixColumns(State& state) {
    State tmp(4, vector<unsigned char>(4));

    for (int j = 0; j < 4; j++) {
        tmp[0][j] = GMul(0x02, state[0][j]) ^ GMul(0x03, state[1][j]) ^ state[2][j] ^ state[3][j];
        tmp[1][j] = state[0][j] ^ GMul(0x02, state[1][j]) ^ GMul(0x03, state[2][j]) ^ state[3][j];
        tmp[2][j] = state[0][j] ^ state[1][j] ^ GMul(0x02, state[2][j]) ^ GMul(0x03, state[3][j]);
        tmp[3][j] = GMul(0x03, state[0][j]) ^ state[1][j] ^ state[2][j] ^ GMul(0x02, state[3][j]);
    }

    state = tmp;
}

// Применение раундового ключа
void AddRoundKey(State& state, const State& roundKey) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] ^= roundKey[i][j];
        }
    }
}

// Шифрования блока
void EncryptBlock(vector<unsigned char>& block, const vector<unsigned char>& key) {
    ofstream stateLog("aes_state_log.txt", ios::out | ios::app);
    State state = ToState(block);
    vector<State> roundKeys = KeyExpansion(key);

    if (stateLog) {
        LogStateToFile(state, stateLog, "Начальное состояние");
        LogStateToFile(roundKeys[0], stateLog, "Ключ раунда 0");
    }

    AddRoundKey(state, roundKeys[0]);
    if (stateLog) LogStateToFile(state, stateLog, "AddRoundKey раунда 0");

    for (int round = 1; round < 10; round++) {
        SubBytes(state);
        if (stateLog) LogStateToFile(state, stateLog, "SubBytes раунда " + to_string(round));

        ShiftRows(state);
        if (stateLog) LogStateToFile(state, stateLog, "ShiftRows раунда " + to_string(round));

        MixColumns(state);
        if (stateLog) LogStateToFile(state, stateLog, "MixColumns раунда " + to_string(round));

        AddRoundKey(state, roundKeys[round]);
        if (stateLog) {
            LogStateToFile(roundKeys[round], stateLog, "Ключ раунда " + to_string(round));
            LogStateToFile(state, stateLog, "AddRoundKey раунда " + to_string(round));
        }
    }

    SubBytes(state);
    if (stateLog) LogStateToFile(state, stateLog, "SubBytes финального раунда");

    ShiftRows(state);
    if (stateLog) LogStateToFile(state, stateLog, "ShiftRows финального раунда");

    AddRoundKey(state, roundKeys[10]);
    if (stateLog) {
        LogStateToFile(roundKeys[10], stateLog, "Ключ финального раунда");
        LogStateToFile(state, stateLog, "AddRoundKey финального раунда");
    }

    block = FromState(state);
    if (stateLog) {
        stateLog.close();
    }
}

// Генерация случайных байтов
void GenerateRandomBytes(vector<unsigned char>& buffer) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);

    for (size_t i = 0; i < buffer.size(); i++) {
        buffer[i] = static_cast<unsigned char>(dis(gen));
    }
}

// Преобразование шестнадцатеричной строки в массив байт
vector<unsigned char> HexStringToBytes(const string& hex) {
    if (hex.length() % 2 != 0) {
        throw invalid_argument("Hex string must have even length");
    }

    vector<unsigned char> bytes(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        bytes[i/2] = static_cast<unsigned char>(stoul(byteString, nullptr, 16));
    }
    return bytes;
}

// Обратная операция относительно функции HexStringToBytes
string BytesToHexString(const vector<unsigned char>& bytes) {
    ostringstream os;
    os << hex << setfill('0');

    for (size_t i = 0; i < bytes.size(); i++) {
        os << setw(2) << static_cast<int>(bytes[i]);
    }

    return os.str();
}

// Шифрование в режиме CFB
void Encrypt(istream& in, ostream& out, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    vector<unsigned char> feedback = iv;

    while (in) {
        vector<unsigned char> plaintext(16, 0);
        vector<unsigned char> ciphertext(16);

        in.read(reinterpret_cast<char*>(plaintext.data()), 16);
        streamsize bytesRead = in.gcount();
        if (bytesRead == 0) break;

        // Encrypt the feedback register
        vector<unsigned char> encryptedFeedback = feedback;
        EncryptBlock(encryptedFeedback, key);

        // XOR with plaintext to get ciphertext
        for (int i = 0; i < 16; i++) {
            ciphertext[i] = plaintext[i] ^ encryptedFeedback[i];
        }

        out.write(reinterpret_cast<char*>(ciphertext.data()), bytesRead);

        // Update feedback register
        if (bytesRead == 16) {
            feedback = ciphertext;
        } else {
            for (int i = 0; i < bytesRead; i++) {
                feedback[i] = ciphertext[i];
            }
        }
    }
}

// Расшифрование в режиме CFB
void Decrypt(istream& in, ostream& out, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    vector<unsigned char> feedback = iv;

    while (in) {
        vector<unsigned char> ciphertext(16, 0);
        vector<unsigned char> plaintext(16);

        in.read(reinterpret_cast<char*>(ciphertext.data()), 16);
        streamsize bytesRead = in.gcount();
        if (bytesRead == 0) break;

        // Encrypt the feedback register
        vector<unsigned char> encryptedFeedback = feedback;
        EncryptBlock(encryptedFeedback, key);

        // XOR with ciphertext to get plaintext
        for (int i = 0; i < 16; i++) {
            plaintext[i] = ciphertext[i] ^ encryptedFeedback[i];
        }

        // Write plaintext
        out.write(reinterpret_cast<char*>(plaintext.data()), bytesRead);

        // Update feedback register
        if (bytesRead == 16) {
            feedback = ciphertext;
        } else {
            for (int i = 0; i < bytesRead; i++) {
                feedback[i] = ciphertext[i];
            }
        }
    }
}

// Обработка ввода вектора инициализации
void IVHandler(vector<unsigned char>& iv) {
    cout << "Введите 32-значное значение IV: ";
    string ivStr;
    cin >> ivStr;

    if (ivStr.length() != 32) {
        cerr << "Длина IV не совпадает с 32 символами!" << endl;
        abort();
    }

    iv = HexStringToBytes(ivStr);
}

// Обработка ввода ключа
void KeyHandler(vector<unsigned char>& key) {
    cout << "Введите 32-значное значение ключа: ";
    string ivStr;
    cin >> ivStr;

    if (ivStr.length() != 32) {
        cerr << "Длина ключа не совпадает с 32 символами!" << endl;
        abort();
    }

    key = HexStringToBytes(ivStr);
}
