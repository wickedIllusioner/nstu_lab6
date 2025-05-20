#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <vector>

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, vector<T> v) {
    for (T elem : v) {
        os << elem << " ";
    }
    return os;
}

int hexToDec(const string& hexStr) {
    int val;
    stringstream ss;
    ss << hex << hexStr;
    ss >> val;
    return val;
}

vector<vector<string>> divideByHexBlocks(const string& msg) {
    stringstream ss;
    vector<vector<string>> vec;

    vector<string> hex_tmp;
    for (auto elem : msg) {
        int ascii_code = elem;
        ss << hex << ascii_code;
        string hexStr = ss.str();
        hex_tmp.push_back(hexStr);
        ss.str(string());

        if (hex_tmp.size() == 4) {
            vec.push_back(hex_tmp);
            hex_tmp.clear();
        }
    }

    if (!hex_tmp.empty()) {
        vec.push_back(hex_tmp);
    }

    return vec;
}

void AESSubstitution(vector<vector<string>>& block) {

}

int main() {
    string input;
    getline(cin, input);
    auto textByBlocks = divideByHexBlocks(input);

    for (auto block : textByBlocks) {
        cout << block << endl;
    }

    return 0;
}
