#include "Feistel.h"
#include "key.h"


void Feistel::CBC_encrypt(Client& c) {
    int padding;
    string iv;

    data_.reserve(3000000);
    hexdata_.reserve(6000000);
    bindata_.reserve(12000000);

    // Input data plain text or file
    initializeData(c);
    char buf[4] = {0};

    padding = addPadding(hexdata_);
    bindata_ = hexToBin(hexdata_);
    int block = 1;

    // Feistel process
    iv = c.get_iv();
    for (int i = 0; i < bindata_.length(); i += 128) {
        string bin = bindata_.substr(i, 128);
        string newbin = XOR_binary(bin, iv);
        string cipherBin = feistel(newbin, c.get_rk());
        cout << "Sent[" << block << "]: "  << binToHex(cipherBin) << endl;
        block++;
        if (i + 128 >= bindata_.length() && !isTxt(c.get_file_name())) {
            cutLastPadding(cipherBin, padding * 4);
        }
        write(c.get_fd(), cipherBin.c_str(), 128);
        read(c.get_fd(), buf, 4);
        iv = cipherBin;
    }
}


void Feistel::CBC_decrypt(User &u, string &bin_data) {
    Key mkey;
    int padding;
    string iv;
    int block = 0;


    iv = u.get_iv();
    string beforeIv = feistel(bin_data, u.get_reverse_round_keys());
    string decryptBin = XOR_binary(beforeIv, iv);
    // If data was from text
    if (bin_data.size() <= 128 && isTxt(u.get_file_name())) {
        decryptBin =  removeTrailingZeros(decryptBin);
    }
    // If data was from other file
    if (bin_data.size() <= 128 && !isTxt(u.get_file_name())) {
        cutLastPadding(decryptBin, padding * 4);
    }
    string decryptHex = binToHex(decryptBin);
    u.append_bin_string(decryptBin);
    u.append_hex_string(decryptHex);
    cout << u.get_ip() << ": " << binToHex(decryptBin) << endl;
    u.set_iv(bin_data);
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


void Feistel::initializeData(Client &c) {
    if (isTxt(c.get_file_name())) {
        data_ = readFile(c.get_file_name());
        hexdata_ = strToHex(data_);
    }
    else {
        hexdata_ = readFile(c.get_file_name());
    }
}


