#include "Feistel.h"
#include "key.h"


Feistel::Feistel() {
    data_.reserve(3000000);
    hexdata_.reserve(6000000);
    bindata_.reserve(12000000);
}


void Feistel::CBC_encrypt(Client& u) {
    int padding;
    string iv;

    // Input data plain text or file
    int fd = u.get_fd();
    initializeData(u);

    padding = addPadding(hexdata_);
    bindata_ = hexToBin(hexdata_);

    // Feistel process
    iv = u.get_iv();
    for (int i = 0; i < bindata_.length(); i += 128) {
        string bin = bindata_.substr(i, 128);
        string newbin = XOR_binary(bin, iv);
        string cipherBin = feistel(newbin, u.get_rk());
        if (i + 128 >= bindata_.length() && !isTxt(u.get_file_name())) {
            cutLastPadding(cipherBin, padding * 4);
        }
        write(u.get_fd(), cipherBin.c_str(), 128);
        iv = cipherBin;
    }

    // Process DD
//    if (!u.get_file_name().empty() && !isTxt(u.get_file_name())) {
//        runDD(u.get_file_name(), u.getOutFile());
//    }
}


void Feistel::CBC_decrypt(User &u) {
    Key mkey;
    int padding;
    string iv;


    hexdata_ = readFile(u.get_file_name());
    if (!isTxt(u.get_file_name())) {
        padding = addPadding(hexdata_);
    }
    bindata_ = hexToBin(hexdata_);

    iv = u.get_iv();

    for (int i = 0; i < bindata_.length(); i += 128) {
        string bin = bindata_.substr(i, 128);
        string beforeIv = feistel(bin, mkey.getRRK());
        string decryptBin = XOR_binary(beforeIv, iv);
        iv = bin;

        // If data was from .txt file
        if (bin.size() <= 128 && isTxt(u.get_file_name())) {
            decryptBin =  removeTrailingZeros(decryptBin);
        }

        // If data was from other file
        if (i + 128 >= bindata_.length() && !isTxt(u.get_file_name())) {
            cutLastPadding(decryptBin, padding * 4);
        }

        if (isTxt(u.get_file_name())) {
            appendToFile(u.get_file_name(), hexToASCII(binToHex(decryptBin)));
        }
        else {
            appendToFile(u.get_file_name(), binToHex(decryptBin));
        }
    }
    if (!isTxt(u.get_file_name())) {
        runDD(u.get_file_name(), u.get_file_name());
    }
}


string Feistel::feistel(const string& bin, const vector<string>& rk) {

    string perm;
    string txt;

    // Permutation1 applied
    for (int i : initial_permutation)
    {
        perm += bin[i - 1];
    }

    // Dividing the result into two equal halves
    string binL = perm.substr(0, 64);
    string binR = perm.substr(64, 128);

    // The plain text is encrypted # of round times
    for (int i = 0; i < 16; i++)
    {
        string right_expand;

        // The right half of the plain text expand // 128
        for (int j : expansion_table)
        {
            right_expand += binR[j - 1];
        }

        // right_expand XOR round key[i]
        string xored = XOR_binary(rk[i], right_expand);


        // Permutation2 is applied
        string perm2;
        for (int j : permutation_tab) {
            perm2 += xored[j - 1];
        }

        // perm2 XOR with the left
        xored = XOR_binary(perm2, binL);

        // The left and the right swapped
        binL = xored;
        if (i < 16 - 1) {
            string temp = binR;
            binR = xored;
            binL = temp;
        }
    }

    // The halves of the plain text are applied
    string combined = binL + binR;

    // Inverse of the initial permutation is applied
    string ciphertext;
    for (int i : inverse_permutation){
        ciphertext += combined[i - 1];
    }

    return ciphertext;
}


void Feistel::initializeData(Client &u) {
    if (isTxt(u.get_file_name())) {
        data_ = readFile(u.get_file_name());
        hexdata_ = strToHex(data_);
    }
    else {
        hexdata_ = readFile(u.get_file_name());
    }
}


