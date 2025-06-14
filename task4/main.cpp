#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// Функция для генерации всех возможных шифров слов длины K
// и подсчета их вхождения
void generate_codes(int k_current, string current_code, const vector<string>& letter_codes, int K, map<string, int>& code_counts) {
    // Базовый случай: если текущая длина слова достигла K
    if (k_current == K) {
        code_counts[current_code]++; // Увеличиваем счетчик для этого сгенерированного шифра
        return;
    }

    // Рекурсивный шаг: для каждой буквы в алфавите
    for (int i = 0; i < letter_codes.size(); ++i) {
        // Добавляем шифр текущей буквы к текущему слову и продолжаем рекурсию
        generate_codes(k_current + 1, current_code + letter_codes[i], letter_codes, K, code_counts);
    }
}

int main() {
    int N, K;
    cin >> N >> K;

    vector<string> letter_codes(N);
    for (int i = 0; i < N; ++i) {
        cin >> letter_codes[i];
    }

    // Map для хранения сгенерированных шифров и их количества
    map<string, int> code_counts;

    // Начинаем генерацию шифров слов с начальной длиной 0 и пустым текущим шифром
    generate_codes(0, "", letter_codes, K, code_counts);

    int unique_decodable_words = 0;
    // Проходим по всем сгенерированным шифрам
    for (const auto& pair : code_counts) {
        // Если шифр встретился только один раз, значит, он соответствует однозначно расшифровываемому слову
        if (pair.second == 1) {
            unique_decodable_words++;
        }
    }

    cout << unique_decodable_words << endl;

    return 0;
}
