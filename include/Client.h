#ifndef COMP7402_PROJECT_CLIENT_H
#define COMP7402_PROJECT_CLIENT_H


#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <csignal>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#import <array>
#include <fstream>
#include <vector>


using namespace std;

class Client {
private:
    string server_ip;
    in_port_t server_port;
    string file_name;
    int file_size;
    int s_fd;

    string data_;
    string hexdata_;
    string bindata_;

    EC_KEY *private_key;
    const EC_POINT *public_key;
    string string_pub;
    string shared_secret_key;
    string iv;
    vector<string> round_keys;


public:
    Client() {
        private_key = nullptr;
        public_key = nullptr;
    }

    void parse_arguments(int argc, char *argv[]);

    void set_fd(int fd) { s_fd = fd; }
    void set_file_size(int fs) { file_size = fs; }
    void set_shared_key(string ssk) { shared_secret_key = std::move(ssk); }
    void set_round_keys(vector<string> rkv) { round_keys = std::move(rkv); }

    string get_ip() const { return server_ip; };
    in_port_t get_port() const { return server_port; }
    string get_shared_secret_key() const { return shared_secret_key; }
    string get_file_name() const { return file_name; }
    int get_file_size() const { return file_size; }
    string get_iv() const { return iv; }
    int get_fd() const { return s_fd; }
    vector<string> get_rk() const { return round_keys; }
    vector<char> read_header(const string& fileName, size_t numBytes);
};


#endif //COMP7402_PROJECT_CLIENT_H
