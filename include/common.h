#ifndef COMP7402_PROJECT_COMMON_H
#define COMP7402_PROJECT_COMMON_H

#include <csignal>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <netinet/in.h>
#include <climits>
#include <string>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <unordered_map>


const int PORT = 53000;
const int MAXEVENTS = 16;
const int BUFFERSIZE = 256;

using namespace std;

in_port_t parse_port(const char *buff, int radix);
void fatal_errno(const char *file, const char *func, size_t line,
                           int err_code, int exit_code);
void fatal_message(const char *file, const char *func, size_t line,
                             const char *msg, int exit_code);

string print_shared_key(const unsigned char* data, size_t length);
int calculate_file_size(const string& filePath);

string hexCharToBin(char hexChar);
string hexToBin(const string& hex);
string binToHex(const string& binary);
string XOR_binary(const string& a, const string& b);
string strToHex(const string& input);
string keyHexToBinary(const string& hex, bool padTo64);
string decToBin(int decimal);
int binToDec(string binary);
string strToBin(const string& input);
void appendToFile(const string& filename, const string& hex);
string readFile(const string& filename);
string readPlainText(const string& prompt);
bool isTxt(const string& filename);
string hexToASCII(const string& hexStr);
string removeTrailingZeros(string binStr);
int addPadding(string& hex);
void runDD(const string& originFileName, const string& encryptedFileName);
int hexCharToValue(char hexChar);
void cutLastPadding(string& binary, int n);
string getFileExtension(const string& filename);
void printDifferenceRate(const string& inFile, const string& outFile);

void overwriteHeader(const std::string& filePath, const std::vector<char>& newHeader);
#endif //COMP7402_PROJECT_COMMON_H
