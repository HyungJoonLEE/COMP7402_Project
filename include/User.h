#ifndef COMP7402_PROJECT_USER_H
#define COMP7402_PROJECT_USER_H

#include <string>
#include <csignal>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <cassert>
#include <iostream>
#include <utility>

using namespace std;

class User {
private:
    int fd_;
    string ip_;

    EC_KEY *server_private_key;              // server private key
    EC_POINT *client_public_key;      // client public key
    EC_POINT *server_public_key;      // server public key

    string server_hex_pub;
    string client_hex_pub;
    string shared_secret_key;
    string file_name;
    string extension;
    string dd;
    int file_size;
    bool file_name_flag = false;
    bool file_size_flag = false;
    bool key_flag = false;

public:
    User() {
        server_private_key = nullptr;
        server_public_key = nullptr;
        client_public_key = nullptr;
    }

    void set_fd(int fd) { fd_ = fd; }
    void set_ip(char* userIP) { ip_ = userIP; }
    void set_server_private_key(EC_KEY * pk) { server_private_key = pk; }
    void set_server_public_key(EC_POINT * pk) { server_public_key = pk; }
    void set_server_hex_pub(string pub) { server_hex_pub = std::move(pub); }
    void set_client_hex_pub(string pub) { client_hex_pub = std::move(pub); }
    void set_shared_secret_key(string ssk) { shared_secret_key = std::move(ssk); }
    void set_key_flag(bool flag) { key_flag = flag; }

    int get_fd() const { return fd_; }
    string get_ip() const { return ip_; }
    string get_server_hex_pup() const { return server_hex_pub; }

    EC_KEY* get_server_private_key() { return server_private_key; }
    EC_POINT* get_server_public_key() { return server_public_key; }
    EC_POINT* get_client_public_key() { return client_public_key; }
    string get_shared_key()  { return shared_secret_key; }
    string get_file_name() const { return file_name; }
    string get_extension() const { return extension; }
    string get_dd() const { return dd; }
    int get_file_size() const { return  file_size; }
    bool is_key_flag() const { return key_flag; }
    bool is_file_flag() const { return file_name_flag; }
    bool is_file_size_flag() const { return file_size_flag; }

    void createPrivateKey();

    void createPublicKey();

    void generateKeys();

};
#endif //COMP7402_PROJECT_USER_H
