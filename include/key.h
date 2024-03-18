#ifndef COMP7402_PROJECT_KEY_H
#define COMP7402_PROJECT_KEY_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Key {
private:
    vector<string> roundKeys_;
    vector<string> reverseRoundKeys_;
public:
    void generateRoundKeys(string key);

    void generateReverseRoundKeys(const vector<string> &rkv, int round);

    static string shift_left_once(string key_chunk);

    static string shift_left_twice(string key_chunk);

    void setRK(const vector<string> &rkv);

    vector<string> getRK() const;

    vector<string> getRRK() const;
};


static int pc1[128] = {
        26, 117, 7, 44, 87, 114, 64, 71,
        116, 72, 101, 4, 75, 5, 69, 10,
        43, 12, 40, 74, 100, 29, 97, 123,
        118, 37, 34, 83, 111, 32, 63, 106,
        121, 79, 52, 13, 35, 17, 91, 31,
        125,126, 62, 80, 58, 11, 119, 77,
        59, 107, 110, 89, 2, 1, 109, 8,
        103, 67, 55, 68, 105, 18, 47, 94,
        30, 115, 90, 46, 81, 84, 41, 20,
        23, 82, 36, 113, 21, 61, 28, 122,
        15, 65, 60, 108, 96, 112, 9, 104,
        76, 124, 56, 39, 53, 16, 85, 92,
        51, 73, 42, 33, 6, 14, 70, 102,
        25,49, 88, 45, 27, 86, 24, 48,
        120, 57,99, 38, 95, 19, 93, 66,
        54, 3, 78,22, 127, 128, 50, 98
};


// The PC2 table
static int pc2[128] = {
        9, 67, 96, 12, 11, 63, 86, 89,
        32, 40, 92, 29, 57, 111, 27, 91,
        94, 98, 126, 65, 44, 28, 58, 97,
        121, 70, 52, 84, 37, 48, 110, 112,
        22, 6, 119, 45, 73, 54, 1, 120,
        62, 55, 30, 83, 107, 8, 117, 104,
        99, 21, 39, 78, 50, 3, 66, 23,
        16, 101, 60, 7, 24, 25, 56, 46,
        77, 13, 33, 81, 68, 100, 26, 114,
        103, 53, 43, 61, 20, 106, 80, 125,
        36, 35, 88, 102, 82, 41, 79, 19,
        14, 127, 85, 128, 10, 109, 95, 108,
        124, 72, 118, 123, 122, 47, 113, 74,
        18, 5, 90, 59, 87, 17, 105, 76,
        64, 34, 38, 42, 69, 2, 49, 71,
        4, 116, 31, 115, 93, 51, 15, 75

};

#endif //COMP7402_PROJECT_KEY_H
