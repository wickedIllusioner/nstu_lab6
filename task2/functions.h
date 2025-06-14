#pragma once

#include <iostream>
#include <vector>

using namespace std;

using State = vector<vector<unsigned char>>;

extern const vector<unsigned char> SBOX;
extern const vector<unsigned char> RCON;

void PrintBlock(const State& state);
void LogStateToFile(const State& state, ofstream& logFile, const string& stage);

unsigned char GMul(unsigned char a, unsigned char b);
State ToState(const vector<unsigned char>& block);
vector<unsigned char> FromState(const State& state);
vector<State> KeyExpansion(const vector<unsigned char>& key);
void SubBytes(State& state);
void ShiftRows(State& state);
void MixColumns(State& state);
void AddRoundKey(State& state, const State& roundKey);
void EncryptBlock(vector<unsigned char>& block, const vector<unsigned char>& key);
void GenerateRandomBytes(vector<unsigned char>& buffer);
vector<unsigned char> HexStringToBytes(const string& hex);
string BytesToHexString(const vector<unsigned char>& bytes);
void Encrypt(istream& in, ostream& out, const vector<unsigned char>& key, const vector<unsigned char>& iv);
void Decrypt(istream& in, ostream& out, const vector<unsigned char>& key, const vector<unsigned char>& iv);

void IVHandler(vector<unsigned char>& iv);
void KeyHandler(vector<unsigned char>& key);
