#ifndef COMP7402_PROJECT_CLIENT_H
#define COMP7402_PROJECT_CLIENT_H


#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
#include "Feistel.h"



using namespace std;

class Client {
private:
    string server_ip;
    in_port_t server_port;
    string file_name;

    EC_KEY *private_key;
    const EC_POINT *public_key;
    string string_pub;
    string shared_secret_key;
    string iv;


public:
    Client() {
        private_key = nullptr;
        public_key = nullptr;
    }

    void parse_arguments(int argc, char *argv[]);
    void print();

    void set_shared_key(string ssk) { shared_secret_key = std::move(ssk); }

    string get_ip() const { return server_ip; };
    in_port_t get_port() const { return server_port; }
    string get_shared_secret_key() const { return shared_secret_key; }
    string get_file_name() const { return file_name; }
    string get_iv() const { return iv; }

    vector<char> read_header(const string& fileName, size_t numBytes);
};


#endif //COMP7402_PROJECT_CLIENT_H
