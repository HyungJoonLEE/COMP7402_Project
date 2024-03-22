#include "key.h"

void Key::generateRoundKeys(string key) {

    // Permute the key using the PC1 table
    string perm_key;
    for (int i : pc1) {
        perm_key += key[i - 1];
    }

    // Dividing the key into two equal halves
    string left  = perm_key.substr(0, 64);
    string right = perm_key.substr(64, 128);

    for(int i = 0; i < 16; i++) {
        // Rounds 1, 2, 9, 16: key_chunks are shifted by one.
        if(i == 0 || i == 1 || i == 8 || i == 15 ){
            left = shift_left_once(left);
            right = shift_left_once(right);
        }

        //  Other rounds: key_chunks are shifted by two
        else {
            left = shift_left_twice(left);
            right = shift_left_twice(right);
        }

        // Combining the two chunks
        string combined_key = left + right;
        string round_key;

        // Using the PC2 table to transpose the key bits
        for (int j : pc2) {
            round_key += combined_key[j - 1];
        }
//        cout << "Round_key " << i << ": " << round_key << endl;
        roundKeys_.push_back(round_key);
    }
}


void Key::generateReverseRoundKeys(const vector<string> &rkv) {
    int i = 15;
    while(i >= 0) {
        reverseRoundKeys_.push_back(rkv[i]);
//        cout << "Round_key " << i << ": " << rkv[i] << endl;
        i--;
    }
}


string Key::shift_left_once(string key_chunk){
    string shifted;

    for(int i = 1; i < 64; i++){
        shifted += key_chunk[i];
    }
    shifted += key_chunk[0];

    return shifted;
}


string Key::shift_left_twice(string key_chunk) {
    string shifted;

    for(int i = 0; i < 2; i++){
        for(int j = 1; j < 64; j++){
            shifted += key_chunk[j];
        }
        shifted += key_chunk[0];
        key_chunk = shifted;
        shifted = "";
    }

    return key_chunk;
}


vector<string> Key::getRK() const {
    return roundKeys_;
}


vector<string> Key::getRRK() const {
    return reverseRoundKeys_;
}
