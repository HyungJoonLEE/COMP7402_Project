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



using namespace std;

class Client {
private:
    string server_ip;
    in_port_t server_port;
    string file_name;
    int file_size;

    EC_KEY *private_key;
    const EC_POINT *public_key;
    string string_pub;
    string shared_secret_key;


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
    int get_file_size() const { return file_size; }
    EC_KEY *get_private_key() const { return private_key; }
    EC_POINT* get_public_key() const { return const_cast<EC_POINT *>(public_key); }
    size_t serialize_ec_point(const EC_GROUP *group, const EC_POINT *point, unsigned char **out);
};


#endif //COMP7402_PROJECT_CLIENT_H
