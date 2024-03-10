#ifndef COMP7402_PROJECT_COMMON_H
#define COMP7402_PROJECT_COMMON_H

#include <csignal>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <netinet/in.h>
#include <climits>
#include <string>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <iomanip>


const int PORT = 53000;
const int MAXEVENTS = 16;
const int BUFFERSIZE = 1024;

using namespace std;

in_port_t parse_port(const char *buff, int radix);
void fatal_errno(const char *file, const char *func, size_t line,
                           int err_code, int exit_code);
void fatal_message(const char *file, const char *func, size_t line,
                             const char *msg, int exit_code);

string print_shared_key(const unsigned char* data, size_t length);


#endif //COMP7402_PROJECT_COMMON_H
