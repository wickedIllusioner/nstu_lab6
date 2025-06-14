#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include "functions.h"

using namespace std;

int main() {
    ofstream clearLog("aes_state_log.txt", ios::out | ios::trunc);
    clearLog.close();

    try {
        int choice;
        string inputFile, outputFile;
        vector<unsigned char> key(16);
        vector<unsigned char> iv(16);

        cout << "AES-128 CFB\n";
        cout << "1. Зашифровать файл\n";
        cout << "2. Расшифровать файл\n";
        cout << "Ваш выбор: ";
        cin >> choice;

        if (choice != 1 && choice != 2) {
            cerr << "Неверное значение. Попробуйте снова\n";
            return 1;
        }

        cout << "Файл для шифровки/расшифровки: ";
        cin >> inputFile;
        cout << "Файл для результата операции: ";
        cin >> outputFile;

        ifstream inFile(inputFile, ios::binary);
        if (!inFile) {
            cerr << "Не удалось открыть файл ввода\n";
            return 1;
        }

        ofstream outFile(outputFile, ios::binary);
        if (!outFile) {
            cerr << "Не удалось открыть файл вывода\n";
            return 1;
        }

        // ШИФРОВАНИЕ
        if (choice == 1) {
            // Открытие файла со случайными ключами
            ofstream keyFile("keys.txt", ios::out);
            if (!keyFile) {
                cerr << "Не удалось внести данные ключей в файл" << endl;
                return 1;
            }

            // Генерация ключа
            GenerateRandomBytes(key);
            cout << "Случайный ключ: " << BytesToHexString(key) << "\n";

            // Генерация IV
            GenerateRandomBytes(iv);
            cout << "Случайный IV: " << BytesToHexString(iv) << "\n";

            string keysToWrite = "KEY: " + BytesToHexString(key) + "\nIV: " + BytesToHexString(iv);
            keyFile.write(keysToWrite.c_str(), keysToWrite.length());
            keyFile.close();

            // Вывод ключей раундов в виде строк
            vector<State> roundKeys = KeyExpansion(key);
            cout << "\nКлючи раундов:\n";
            for (size_t i = 0; i < roundKeys.size(); ++i) {
                cout << "Раунд " << setw(2) << i << ": ";
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        cout << hex << setw(2) << setfill('0')
                             << static_cast<int>(roundKeys[i][k][j]);
                        cout << dec;
                    }
                }
                cout << endl;
            }

            Encrypt(inFile, outFile, key, iv);
            cout << "\nФайл успешно зашифрован" << endl;
            cout << "Сгенерированные значения были записаны в файл keys.txt" << endl;
            cout << "Промежуточные состояние State записаны в файл aes_state_log.txt" << endl;

            keyFile.close();
        }

        // ДЕШИФРОВАНИЕ
        else {
            KeyHandler(key);
            IVHandler(iv);

            Decrypt(inFile, outFile, key, iv);
            cout << "Файл успешно расшифрован\n";
        }

        inFile.close();
        outFile.close();

    } catch (const exception& e) {
        cerr << "Произошла ошибка: " << e.what() << "\n";
        abort();
    }

    return 0;
}
