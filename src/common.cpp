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


string extract_public_key(int socketfd) {
    char buf[128] = {0};
    read(socketfd, buf, 255);

    string bufStr(buf); // Convert the char array to a string
    string prefix = "[public_key]";
    cout << "server: " << buf << endl;
    size_t startPos = bufStr.find(prefix);
    if (startPos != string::npos) {
        startPos += prefix.length() + 1; // +1 for the space after the prefix

        // Extract the substring starting from startPos to the end of the input string
        string pk = bufStr.substr(startPos);
        return pk;
    }
    else {
        cout << "Prefix '[public_key]' not found in the input string." << endl;
        return "";
    }
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


int get_file_size(const string& filePath) {
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


string keyHexToBinary(const string& hex, bool padTo64) {
    string binaryString;
    for (size_t i = 0; i < hex.length(); ++i) {
        char c = hex[i];

        int value = (c >= '0' && c <= '9') ? (c - '0') : (tolower(c) - 'a' + 10);
        for (int j = 3; j >= 0; j--) {
            binaryString.push_back(((value >> j) & 1) ? '1' : '0');
        }
    }

    if (padTo64 && binaryString.length() < 64) {
        string padded(64 - binaryString.length(), '0');
        binaryString = padded + binaryString;
    }
    return binaryString;
}


string decToBin(int decimal) {
    string binary;
    while(decimal != 0) {
        binary = (decimal % 2 == 0 ? "0" : "1") + binary;
        decimal = decimal/2;
    }
    while(binary.length() < 4){
        binary = "0" + binary;
    }
    return binary;
}


int binToDec(string binary) {
    int decimal = 0;
    int counter = 0;
    int size = binary.length();

    for(int i = size-1; i >= 0; i--) {
        if(binary[i] == '1') {
            decimal += pow(2, counter);
        }
        counter++;
    }
    return decimal;
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
        file << hex;

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


string readPlainText(const string& prompt) {
    string input;

    cout << prompt;
    getline(cin, input);
    return input;
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
    int padding = 16 - (int) hex.size() % 16;
    if (padding == 0) return 0;
    for (int i = 0; i < padding; ++i) {
        hex += "0";
        count++;
    }
    return count;
}


void printRoundKeys(vector<string> rks) {
    cout << "< Round Keys >" << endl;
    for (int i = 0; i < rks.size(); i++) {
        cout << hex << "Round Key " << i + 1 << " : " << rks[i]  << endl;
    }
}


void runDD(const string& originFileName, const string& encryptedFileName) {
    int hdrLen = fileType[getFileExtension(originFileName)];
    string command = "dd if=" + originFileName + " of=" + encryptedFileName +
                     " bs=1 count=" + to_string(hdrLen) + " conv=notrunc";
    int status = system(command.c_str());
    if (status != 0) {
        cerr << "Command failed with status: " << status << endl;
    }
    cout << endl;
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


string getFileExtension(const string& filename) {
    size_t dotPos = filename.find_last_of('.');

    if (dotPos != string::npos) {
        string ext = filename.substr(dotPos + 1);
        return ext;
    }
    else
        return "";
}


void printDifferenceRate(const string& inFile, const string& outFile) {
    string data, bin1, bin2;

    bin1.reserve(200000);
    bin2.reserve(200000);

    string f1 = readFile(inFile);
    string f2 = readFile(outFile);

    if (isTxt(inFile)) {
        data = hexToASCII(f1);
        bin1 = strToBin(data);
        bin2 = strToBin(f2);
    }
    else {
        bin1 = hexToBin(f1);
        bin2 = hexToBin(f2);
    }


    double count;
    unsigned long len = bin2.length();

    for (int i = 0; i < len; i++) {
        if (bin1[i] != bin2[i]) count++;
    }

    cout << fixed << setprecision(3) << "Difference rate: " << count / (double)len * 100 << "%" << endl;
}