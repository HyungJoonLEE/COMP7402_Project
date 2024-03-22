#include "common.h"

unordered_map<string, int> fileType {
        {"bmp", 54},
};


in_port_t parse_port(const char *buff, int radix) {
    char *end;
    long sl;
    in_port_t port;
    const char *msg;

    sl = strtol(buff, &end, radix);

    if (end == buff) {
        msg = "not a decimal number";
    } else if (*end != '\0') {
        msg = "%s: extra characters at end of input";
    } else if ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) {
        msg = "out of range of type long";
    } else if (sl > UINT16_MAX) {
        msg = "greater than UINT16_MAX";
    } else if (sl < 0) {
        msg = "less than 0";
    } else {
        msg = NULL;
    }

    if (msg) {
        fatal_message(__FILE__, __func__, __LINE__, msg, 2);
    }

    port = (in_port_t) sl;

    return port;
}


void fatal_errno(const char *file, const char *func, size_t line,
                           int err_code, int exit_code) {
    const char *msg;

    msg = strerror(err_code); // NOLINT(concurrency-mt-unsafe)
    fprintf(stderr, "Error (%s @ %s:%zu %d) - %s\n", file, func, line, err_code,
            msg);    // NOLINT(cert-err33-c)
    exit(exit_code); // NOLINT(concurrency-mt-unsafe)
}


void fatal_message(const char *file, const char *func, size_t line,
                             const char *msg, int exit_code) {
    fprintf(stderr, "Error (%s @ %s:%zu) - %s\n", file, func, line,
            msg);    // NOLINT(cert-err33-c)
    exit(exit_code); // NOLINT(concurrency-mt-unsafe)
}


string print_shared_key(const unsigned char* data, size_t length) {
    ostringstream oss;
    oss << hex << uppercase << setfill('0');
    for (size_t i = 0; i < length; ++i) {
        oss << setw(2) << static_cast<int>(data[i]);
    }
    return oss.str();
}


int calculate_file_size(const string& filePath) {
    int file_size;
    ifstream file(filePath, ifstream::binary | ifstream::ate);
    if (!file) {
        cout << "File doesn't exist" << endl;
        return -1;
    }

    if (file.tellg() >= 0 && file.tellg() <= numeric_limits<int>::max()) {
        file_size = static_cast<int>(file.tellg());
    }

    return file_size;
}


string hexCharToBin(char hexChar) {
    switch(hexChar) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': case 'a': return "1010";
        case 'B': case 'b': return "1011";
        case 'C': case 'c': return "1100";
        case 'D': case 'd': return "1101";
        case 'E': case 'e': return "1110";
        case 'F': case 'f': return "1111";
        default: return "";
    }
}


string hexToBin(const string& hexStr) {
    string binStr;
    for (char hexChar : hexStr) {
        binStr += hexCharToBin(hexChar);
    }
    return binStr;
}


string binToHex(const string& binary) {
//    string hexStr;
//    for (size_t i = 0; i < binary.length(); i += 8) {
//        bitset<8> eightBits(binary.substr(i, 8));
//        stringstream ss;
//        ss << hex << setw(2) << setfill('0') << eightBits.to_ulong();
//        hexStr += ss.str();
//    }
//    return hexStr;
    stringstream ss;
    for (size_t i = 0; i < binary.length(); i += 4) {
        bitset<4> bs(binary.substr(i, 4));
        ss << hex << bs.to_ulong();
    }
    return ss.str();
}


string XOR_binary(const string& a, const string& b) {
    string result;
    for (size_t i = 0; i < a.size(); ++i) {
        result.push_back((a[i] == b[i]) ? '0' : '1');
    }
    return result;
}


string strToHex(const string& input) {
    stringstream hexStream;

    for (unsigned char c : input) {
        hexStream << hex << setw(2) << setfill('0') << static_cast<int>(c);
    }

    return hexStream.str();
}


string strToBin(const string& input) {
    return hexToBin(strToHex(input));
}


void appendToFile(const string& filename, const string& hex) {
    if (isTxt(filename)) {
        ofstream file(filename, ios::out | ios::app);
        if (!file) {
            cerr << "Failed to open file for writing." << endl;
            return;
        }
        file << hexToASCII(hex);

        file.close();
    }
    else {
        ofstream file(filename, ios::app | ios::binary);
        if (!file) {
            cerr << "Failed to open file for writing." << endl;
            return;
        }

        for (size_t i = 0; i < hex.length(); i += 2) {
            // Convert each pair of hex digits to a byte
            int value = hexCharToValue(hex[i]) * 16 + hexCharToValue(hex[i + 1]);
            char byte = static_cast<char>(value);
            file.write(&byte, 1);
        }
        file.close();
    }
}


string readFile(const string& filename) {
    if (isTxt(filename)) {
        ifstream file(filename);
        string line, data;

        line.reserve(1000000);
        data.reserve(2000000);

        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return "";
        }

        // Read file line by line
        while (getline(file, line)) {
            data += line;
            data += '\n';
        }

        file.close();
        return data;
    }

    else {
        ifstream file(filename, ios::binary);
        if (!file) {
            cerr << "Cannot open file." << endl;
            return "";
        }

        vector<unsigned char> buffer(istreambuf_iterator<char>(file), {});

        stringstream hexStream;

        for (unsigned char byte : buffer) {
            hexStream << hex << setw(2) << setfill('0') << static_cast<int>(byte);
        }

        file.close();

        return hexStream.str();
    }
}


bool isTxt(const string& filename) {
    size_t dotPosition = filename.find_last_of('.');

    if (dotPosition != string::npos) {
        string extension = filename.substr(dotPosition + 1);
        return extension == "txt";
    }
    return false;
}


string hexToASCII(const string& hexStr) {
    string asciiStr;
    for (size_t i = 0; i < hexStr.length(); i += 2) {
        string part = hexStr.substr(i, 2);
        char ch = static_cast<char>(stoi(part, nullptr, 16));
        asciiStr += ch;
    }
    return asciiStr;
}


string removeTrailingZeros(string binStr) {
    string result;
    string pattern = "00000000"; // 0s to remove
    for (size_t i = 0; i < binStr.length(); ) {
        if (i + 8 <= binStr.length() && binStr.substr(i, 8) == pattern) {
            i += 8;
        } else {
            result += binStr[i];
            i++;
        }
    }
    return result;
}


int addPadding(string& hex) {
    int count;
    int padding = (int) hex.size() % 32;
    if (padding == 0) return 0;
    for (int i = 0; i < 32 - padding; ++i) {
        hex += "0";
        count++;
    }
    return count;
}


int hexCharToValue(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9') return hexChar - '0';
    if (hexChar >= 'a' && hexChar <= 'f') return 10 + (hexChar - 'a');
    if (hexChar >= 'A' && hexChar <= 'F') return 10 + (hexChar - 'A');
    return 0;
}


void cutLastPadding(string& binary, int n) {
    if (n <= 0) return;

    if (n > static_cast<int>(binary.size())) {
        binary.clear();
    }
    else {
        binary.erase(binary.size() - n);
    }
}
