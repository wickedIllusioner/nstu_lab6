#pragma once
using namespace std;

vector<vector<string>> AddRoundKey(vector<vector<string>> state, vector<vector<string>> roundkey);
void SubBytes(vector<vector<string>>& state);
void ShiftRows(vector<vector<string>>& state);
void MixColumn(vector<vector<string>>& state, vector<vector<string>> fixed);
